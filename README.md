# MICCING

This is my working copy of ICCING, the initial-condition code that samples gluon splitting into quark-antiquark pairs to put conserved charges (baryon, strangeness, electric charge) on top of a TRENTo-type energy/entropy profile. The original code is by Patrick Carzon and collaborators (the-nuclear-confectionery/ICCING). Everything before my commits is their work, I'm building on top of it.

What I'm after: I want to know where each individual quark ends up and what momentum it carries, not just the smeared BSQ densities. So most of what I've added is about keeping track of the quarks one by one and giving them momenta from an anisotropic distribution.

## What I changed so far

### 1. Per-quark output (`quark_list.dat`)

Every time a gluon splits, I now store both the quark and the antiquark (position, flavor, mass, charges) along with the position of the parent gluon before the split and the splitting fraction alpha. At the end of each event this gets written to `quark_list.dat`, one line per quark:

```
event  x  y  flavor  mass  B  S  Q  is_antiquark  gluon_x  gluon_y  alpha  Lambda  px  py  pz  vx  vy  vz
```

- x, y, gluon_x, gluon_y in fm (same grid conversion the code already uses)
- flavor is up / down / strange / charm, mass in GeV
- is_antiquark is 0 for the quark and 1 for the antiquark (they always come in pairs, quark first)
- Lambda in GeV, momenta in GeV, v = p/E

I also reset the quark counters in `CleanEvent()`, just to be safe (right now every event starts from a fresh copy so it doesn't change any output).

### 2. Momenta from a Romatschke-Strickland distribution

The quarks now get momenta sampled from the RS distribution with Boltzmann statistics,

f(p) ~ exp( -sqrt(pT^2 + (1+xi) pz^2 + m^2) / Lambda ),

with xi fixed at 4 for now (strongly oblate, like early-time pre-equilibrium).

How Lambda is fixed: at startup (`InitializeMomentumDistribution()`) I compute the energy density as a function of Lambda,

epsilon(Lambda) = g ∫ d^3p E exp(-E_xi / Lambda),

where E is the real on-shell energy and only the Boltzmann weight is stretched with xi. g = 6 (spin x color). The angle and pz-parity are done analytically and the remaining pT, pz double integral is done numerically with GSL. I do this on a log grid of 150 Lambda values between 0.02 and 2.5 GeV for each flavor (and for the massless gluon), and spline-invert it to get Lambda(epsilon). Then in each event I just look up Lambda from the local energy density.

Sampling: the RS distribution is just an isotropic Boltzmann distribution in the stretched variable pz' = sqrt(1+xi) pz. So I sample |p'| isotropically by rejection sampling, pick a random direction, and then un-stretch pz at the end.

Momentum conservation for the pair: at first I was sampling the quark and the antiquark independently, but then their momenta don't add up to anything physical. Now I sample one momentum for the parent gluon (massless, at its position before splitting, using the local energy density there) and split it with alpha: the quark gets alpha p_g and the antiquark gets (1-alpha) p_g. This is the same alpha the code already uses to split the gluon's energy. This has to run after the density grid for the event is built, so it's called in `WriteEvent()`.

### 3. EOS reading bug

Heads up, this one matters for anyone using `derv21.dat`. The header of the EOS file says 1993 rows but there are only 1992 lines of data. The old loop trusted the header, so it read past the end of the file, and since a failed read just gives 0, it silently added a fake (energy = 0, entropy = 0) point at the end of the table. The cubic spline is solved over the whole table at once, so that one garbage point can mess up a good part of the spline, not just the last interval. I changed the loop to read until the file actually ends. I didn't touch the data file itself. The old loop is still there commented out in `io.cpp` in case I need to go back.

### 4. Temperature and a velocity column in the density output

From the EOS spline I now also build T(epsilon), using T = de/ds (that's just the first-derivative coefficient of the spline at each knot). I use this to add a 7th column to the sparse density grid output: a rough velocity for each cell, v = sqrt(1 - m^2/E^2) with E = 2.7 T, where m is 0.095 GeV if the cell has strangeness, 0.0035 GeV (average of up and down) if it only has baryon number, and 0 (so v = 1) if it's gluon dominated. This is a crude estimate and I'll probably replace it with something based on the actual quark momenta from point 2.

Note: this changes the format of the density output files (6 columns -> 7), so old scripts that read them will need updating.

### 5. Small fixes

- `SplineSet::operator=` was missing `return *this;` (undefined behavior)
- `q_s` and `e_tot` in `Event::GetGlue()` are now initialized to 0
- the EOS splines are now copied in `IO::CopyIO()`
- quark_list is copied/cleared along with the rest of the event

### 6. Config

`ICCING_GEN.conf` now points to local paths (`./Input/`, output in `./`) and runs just event 46 with output_type 1, for testing on my laptop. I also renamed `e_chop` to `s_chop` in the config, since `s_chop` is what the code actually reads. With `e_chop` it's worse than being ignored: an unknown key goes through `mapConfigParams[...]`, which quietly creates it with value 0, and 0 is `trentoinputdir` in the enum. So that line was overwriting the TRENTo input directory with "10E-20" instead of hitting the "Unknown Parameter" error.

## Building and running

Needs GSL and a compiler with OpenMP. On my Mac (Homebrew gcc):

```
make CC=g++-15 iccing
./iccing ICCING_GEN.conf
```

On Linux plain `make iccing` should work.

## Things I still want to do / known limitations

- xi is hard coded to 4. It should be a config parameter, and ideally depend on time or position.
- Boltzmann statistics only, no Fermi-Dirac / Bose-Einstein yet.
- the quark masses are written in two places (`global.h` and `InitializeMomentumDistribution()`), they need to stay in sync.
- the velocity column in the density files is a rough estimate (see point 4).
- make the config reader actually reject unknown keys (use `find` instead of `operator[]` on the map).
- check the momentum distributions against the analytic RS moments (pT and pz averages) as a test.
