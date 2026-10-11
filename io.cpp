#include "io.h"
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

//__________________________________________________________________________________________
//##########################################################################################
//  Class constructor
//    Reads in config file and initializes variables
//##########################################################################################
IO::IO(string configFile)
{
  Initialize(); // Initialze config params and map for reading in config params

  //  config input stream
  ifstream input;
  input.open(configFile);

  string var_type;  //  Used to record parameter type read from file

  //******************************************************************************************
  //  Loop through config file
  //******************************************************************************************
  while (!input.eof())
	{
    input >> var_type;  //  Read parameter type

    //  Switch through the possible parameter types
    //  uses var_type as key to map then reads value to class variable
    switch(mapConfigParams[var_type])
    {
      //  case ConfigParam (Does var_type map to ConfigParam?)
      //  input >> input_var (Read in value of var_type)
      //  break; (Stop checking switch and move on)
      case trentoinputdir: input >> trento_input_dir; break;
      case quarkinputfile: input >> quark_input_file; break;
      case eosfile: input >> eos_file; break;
      case outputdir:  input >> output_dir; break;
      case inputtype: input >> input_type; break;
      case outputtype:  input >> output_type; break;
      case seed: input >> seed_; break;
      case test: input >> test_; break;

      case eventlabel: input >> event_label; break;
      case firstevent: input >> first_event; break;
      case lastevent: input >> last_event; break;
      case ta:  input >> t_a; break;
      case tb:  input >> t_b; break;

      case kappa:  input >> kappa_; break;
      case rad:  input >> rad_; break;
      case qrad:  input >> qrad_; break;
      case lambda:  input >> lambda_; break;

      case dipolemodel:  input >> dipole_model; break;
      case alphas:  input >> alpha_s; break;
      case alphamin:  input >> alpha_min; break;
      case rmax:  input >> r_max; break;
      case lambdabym: input >> lambda_bym; break;

      case atrento:  input >> a_trento; break;
      case schop:  input >> s_chop; break;

      case gridmax: input >> grid_max;  break;
      case gridstep:  input >> grid_step; break;
      case tau0:  input >> tau_0; break;
      case ethresh:  input >> e_thresh; break;
      case chargetype:  input >> charge_type; break;

      //  Error statement, triggered if unknown parameter is specified in config, exits program
      default: cout << "Error Unknown Parameter Used" << endl; exit(0); break;

      //#CONFIGPARAM
    }// End of switch
  }// End of while loop

  if (seed_ == 0)
  {
    seed_ = time(NULL);
    get_random_number.seed(seed_);  //  Set random seed from input or timestamp
  }

  //  Setting flag for type of charge tracked
  if(charge_type == "BSQ") {  tracked_charge = 0; }
  else if (charge_type == "UDS") {  tracked_charge = 1; }

  current_event = first_event;  //  Set current_event to first_event

  grid_points = 2*(grid_max/grid_step); //  Calculate # grid_points

  // Output all values, specified and unspecified, used by run of code
  OutputConfig(output_dir + "run_parameters" + to_string(current_event) + ".dat");
}// End of Class constructor
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Class Destructor
//##########################################################################################
IO::~IO()
{

}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  IO Copy Function
//##########################################################################################
void IO::CopyIO(const IO &e)
{
  trento_input_dir = e.trento_input_dir;
  quark_input_file = e.quark_input_file;
  eos_file = e.eos_file;
  output_dir = e.output_dir;
  input_type = e.input_type;
  output_type = e.output_type;
  seed_ = e.seed_;
  test_ = e.test_;

  event_label = e.event_label;
  first_event = e.first_event;
  last_event = e.last_event;
  t_a = e.t_a;
  t_b = e.t_b;

  kappa_ = e.kappa_;
  rad_ = e.rad_;
  qrad_ = e.qrad_;
  lambda_ = e.lambda_;

  dipole_model = e.dipole_model;
  alpha_s = e.alpha_s;
  alpha_min = e.alpha_min;
  r_max = e.r_max;
  lambda_bym = e.lambda_bym;

  a_trento = e.a_trento;
  s_chop = e.s_chop;
  eos_interped = e.eos_interped;
  temp_from_energy = e.temp_from_energy;

  grid_max = e.grid_max;
  grid_step = e.grid_step;
  tau_0 = e.tau_0;
  e_thresh = e.e_thresh;
  charge_type = e.charge_type;
  //#CONFIGPARAM

  tracked_charge = e.tracked_charge;
  current_event = e.current_event;
  grid_points = e.grid_points;
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Implicit Copy
//##########################################################################################
IO::IO(const IO &original)
{
  CopyIO(original);
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Overide operator=
//##########################################################################################
IO& IO::operator= (const IO& original)
{
	CopyIO(original);
	return *this;
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Initialze variables and map
//##########################################################################################
void IO::Initialize()
{
  //******************************************************************************************
  //  Set variables to default values
  //******************************************************************************************
  trento_input_dir = "";
  quark_input_file = "";
  eos_file = "";
  output_dir = "";
  input_type = 1;
  output_type = 1;
  seed_ = 0;
  test_ = "";

  event_label = "";
  first_event = 0;
  last_event = 0;
  t_a = false;
  t_b = true;

  kappa_ = 1.0;
  rad_ = 0.5;
  qrad_ = 0.5;
  lambda_ = 1.0;

  dipole_model = "";
  alpha_s = 0.3;
  alpha_min = 0.01;
  r_max = 1.0;
  lambda_bym = 0.0;

  a_trento = 119.0;
  s_chop = 10.0E-20;

  grid_max = 12.0;
  grid_step = 0.06;
  tau_0 = 0.6;
  e_thresh = 0.25;
  charge_type = "BSQ";
  //#CONFIGPARAM

  tracked_charge = 0;
  current_event = 0;
  grid_points = 0;

  //******************************************************************************************
  //  Initialze map for reading in config file
  //******************************************************************************************
  mapConfigParams["trento_input_dir"] = trentoinputdir;
  mapConfigParams["quark_input_file"] = quarkinputfile;
  mapConfigParams["eos_file"] = eosfile;
  mapConfigParams["output_dir"] = outputdir;
  mapConfigParams["input_type"] = inputtype;
  mapConfigParams["output_type"] = outputtype;
  mapConfigParams["seed_"] = seed;
  mapConfigParams["test_"] = test;

  mapConfigParams["event_label"] = eventlabel;
  mapConfigParams["first_event"] = firstevent;
  mapConfigParams["last_event"] = lastevent;
  mapConfigParams["t_a"] = ta;
  mapConfigParams["t_b"] = tb;

  mapConfigParams["kappa_"] = kappa;
  mapConfigParams["rad_"] = rad;
  mapConfigParams["qrad_"] = qrad;
  mapConfigParams["lambda_"] = lambda;

  mapConfigParams["dipole_model"] = dipolemodel;
  mapConfigParams["alpha_s"] = alphas;
  mapConfigParams["alpha_min"] = alphamin;
  mapConfigParams["r_max"] = rmax;
  mapConfigParams["lambda_bym"] = lambdabym;

  mapConfigParams["a_trento"] = atrento;
  mapConfigParams["s_chop"] = schop;

  mapConfigParams["grid_max"] = gridmax;
  mapConfigParams["grid_step"] = gridstep;
  mapConfigParams["tau_0"] = tau0;
  mapConfigParams["e_thresh"] = ethresh;
  mapConfigParams["charge_type"] = chargetype;
  //#CONFIGPARAM
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Copy config file to ICCING output folder for future reference
//##########################################################################################
void IO::OutputConfig(string file_name)
{
  ofstream output;
  output.open(file_name);

  output
    << "trento_input_dir " << trento_input_dir
    << "\noutput_dir " << output_dir
    << "\ninput_type " << input_type
    << "\noutput_type " << output_type
    << "\nseed_ " << seed_
    << "\ntest_ " << test_;

  output
    << "\n\nevent_label " << event_label
    << "\nfirst_event " << first_event
    << "\nlast_event " << last_event
    << "\nt_a " << t_a
    << "\nt_b " << t_b;

  output
    << "\n\nkappa_ " << kappa_
    << "\nrad_ " << rad_
    << "\nqrad_ " << qrad_
    << "\nlambda_ " << lambda_;

  output
    << "\n\ndipole_model " << dipole_model
    << "\nalpha_s " << alpha_s
    << "\nalpha_min " << alpha_min
    << "\nr_max " << r_max
    << "\nlambda_bym " << lambda_bym;
  output
    << "\n\na_trento " << a_trento
    << "\ns_chop " << s_chop;
  output
    << "\ngrid_max " << grid_max
    << "\ngrid_step " << grid_step
    << "\ntau_0 " << tau_0
    << "\ne_thresh " << e_thresh
    << "\ncharge_type " << charge_type;
    //#CONFIGPARAM

    output.close();
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Initialize the Event Object
//##########################################################################################
Event IO::InitializeEvent()
{
  Event event_in; //  Temp Event object used to store event specific data

  //  Set variables in event with data from configFile
  event_in.kappa_ = kappa_;
  event_in.lambda_ = lambda_;
  event_in.tau_0 = tau_0;
  event_in.e_thresh = e_thresh;
  event_in.kappa_ = kappa_;
  event_in.grid_max = grid_max;
  event_in.grid_step = grid_step;
  event_in.grid_points = grid_points;
  event_in.test_ = test_;

  //  Initialize input grid to 0 with dimensions grid_points + 1
  event_in.initial_energy.resize(grid_points + 1, vector<double>(grid_points + 1, 0.));

  for (int i = 0; i < 4; i++)
  {
    event_in.density.push_back(event_in.initial_energy);
  }

  //******************************************************************************************
  //  Initialze Gluon Distribution for sampling
  // `  Using this simple mask, makes repeat calculations quick and easy since there is no
  //    need to calculate the circle multiple times.
  //******************************************************************************************
  event_in.gluon_rad = round(rad_/grid_step); //  Set radius of gluons in grid points
  //  Set size of gluon_dist grid used to sample energy from initial_energy
  event_in.gluon_dist.resize(2*event_in.gluon_rad + 1, vector<int>(2*event_in.gluon_rad + 1, 0.));

  //  Initialize gluon distribution
  int ox = event_in.gluon_rad;  //  x-value of gluon_dist center
  int oy = event_in.gluon_rad;  //  y-value of gluon_dist center

  //  Loop through only points in radius of gluon and set to 1
  for (int i = -event_in.gluon_rad; i <= event_in.gluon_rad; i++) //  This goes -radius to radius in x
  {
    // This calculates the height of the gluon_dist at a given x-value
    int height = round(sqrt(event_in.gluon_rad*event_in.gluon_rad - i*i));
    for (int j = -height; j <= height; j++) //  This loops over the points in circle at given x
    {
      event_in.gluon_dist[i + ox][j + oy] = 1;  //  Set points in circle to 1 for calculations
    }
  }

  //******************************************************************************************
  //  Calculate Quark distribution for depositing densities
  //    This gaussian weighted mask simplifies and streamlines the deposit of quarks, since
  //    there is no need to repeat the normalization and profile calculation.
  //    This mask gets weighted by quark charges and energy for deposit in output.
  //******************************************************************************************
  event_in.quark_rad = round(qrad_/grid_step); //  Set radius of quarks
  //  Set size of quark_dist grid used to create quarks
  event_in.quark_dist.resize(2*event_in.quark_rad + 1, vector<double>(2*event_in.quark_rad + 1, 0.));

  //  Initialize quark distribution
  int ox_quark = event_in.quark_rad;  //  x-value of quark_dist center
  int oy_quark = event_in.quark_rad;  //  y-value of quark_dist center
  double point;
  double normalization = 0;

  //  Loop through only points in radius of quark to get normalization factor
  for (int i = -event_in.quark_rad; i <= event_in.quark_rad; i++)  //  This goes -radius to radius in x
  {
    // This calculates the hight of the quark_dist at a given x-value
    int height = round(sqrt(event_in.quark_rad*event_in.quark_rad - i*i));
    for (int j = -height; j <= height; j++) //  This loops over the points in circle at given x
    {
      point = sqrt(pow((i),2) + pow((j),2));  //  Get distance of point from center of circle
      normalization += exp(-((pow(point,2))/(2*pow(event_in.quark_rad,2))));  //  Add value at poinnt to a normalization factor
    }
  }

  //  Loop through only points in radius of quark to get normalization factor
  for (int i = -event_in.quark_rad; i <= event_in.quark_rad; i++)
  {
    // This calculates the hight of the quark_dist at a given x-value
    int height = round(sqrt(event_in.quark_rad*event_in.quark_rad - i*i));
    for (int j = -height; j <= height; j++) //  This loops over the points in circle at given x
    {
      point = sqrt(pow(i,2) + pow(j,2));  //  Get distance of point from center of circle
      //  Calculate value of gaussian at point in circle
      event_in.quark_dist[i + ox_quark][j + oy_quark] = 1/(normalization*pow(grid_step,2)*tau_0)*exp(-((pow(point,2))/(2*pow(event_in.quark_rad,2))));
    }
  }

  return event_in;
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Initialize the Splitter Object
//##########################################################################################
Splitter IO::InitializeSplitter()
{
  // Add Comment lots of comments needed here

  Splitter init_splitter; //  Temp Splitter object used to store splitter specific data

  //  Set variables in splitter with data from configFile
  if (charge_type == "BSQ")  { init_splitter.charge_type = 0;  }
  if (charge_type == "UDS")  { init_splitter.charge_type = 1;  }
  init_splitter.alpha_s = alpha_s;
  init_splitter.alpha_min = alpha_min;
  init_splitter.r_max = r_max;
  init_splitter.e_thresh = e_thresh;
  init_splitter.lambda_ = lambda_;
  init_splitter.grid_step = grid_step;
  init_splitter.test_ = test_;
  init_splitter.output_dir = output_dir;

  //  Initialization of correlator function with correct dipole_model
  init_splitter.Model_Correlator = Correlator(dipole_model, lambda_bym, alpha_s);

  //******************************************************************************************
  //  Read in quark chemistry file for use in splitting probabilities
  //    This is an interpolation function used to get the splitting probability for a given
  //    quark and q_s.
  //******************************************************************************************
  double value;
  vector<double> ratio_q_s;
  vector<vector<double>> ratio_quarks(4); // 0 = up, 1 = down, 2 = strange, 3 = charm
  init_splitter.flavor_chemistry.resize(4);

  ifstream input;
  input.open(quark_input_file);

  //  Read each line of file and add flavor probabilities for given q_s at specified index
  while (!input.eof())
  {
    //  Read in q_s value and add to the end of q_s list
    input >> value;
    ratio_q_s.push_back(value);

    //  Read in the 4 different flavor probabilities and add them to the end of their
    //  respective lists
    for (int i = 0; i < 4; i++)
    {
      input >> value;
      ratio_quarks[i].push_back(value);
    }
  }

  //  Initialize Cubic Spline interpolators for each set of flavor with the same list of q_s
  for (int i = 0; i < 4; i++)
  {
    init_splitter.flavor_chemistry[i] = CubicSpline(ratio_q_s, ratio_quarks[i]);
  }

  return init_splitter;
}
//__________________________________________________________________________________________


//__________________________________________________________________________________________
//##########################################################################################
// Initialize equation of state
//##########################################################################################
void IO::InitializeEOS()
{
  ifstream input;
  input.open(eos_file);

  vector<double> energy;
  vector<double> entropy;
  double value;

  int length;
  input >> length;

  //  heads up: the eos file header said 1993 rows but there were only 1992 actual data lines in it.
  //  so the old for-loop (which trusted that header number) was reading past the end of the file for
  //  the last row, and since failed reads just silently give you 0 in c++11+, it was quietly shoving
  //  a fake (energy=0, entropy=0) point onto the end of the table, after entropy had already gone up
  //  to like 1169. that broke the cubic spline because the spline solve is coupled across the whole
  //  table, not just locally, so one garbage point at the end could mess with a bunch of it going
  //  backward too. fix: stop trusting the header number for how many rows to read, just read until
  //  the file actually runs out. didn't touch the data file itself, only this loop, so nothing else
  //  that reads derv21.dat is affected.
  //******************************************************************************************
  //  For each line of EOS file add correlated energy, temperature, and entropy values to end
  //  of respective list
  //******************************************************************************************
  //  old version, kept here commented out in case i need to go back to it - if so, just comment out
  //  the while-loop below and uncomment this one.
  //  for (int i = 0; i < length; i++)
  //  {
  //    input >> value;
  //    energy.push_back(value);
  //
  //    input >> value;
  //
  //    input >> value;
  //    entropy.push_back(value);
  //
  //    input >> value;
  //  }
  while (input >> value)
  {
    energy.push_back(value);

    input >> value;

    input >> value;
    entropy.push_back(value);

    input >> value;
  }

  input.close();

  //  Initialze Cubic Spline for use in converting initial condition from entropy to energy
  eos_interped = CubicSpline(entropy, energy);

  //  Build energy -> temperature spline using T = de/ds = b coefficient at each knot
  //  eos_interped[i].b is the first derivative (de/ds = T) at the i-th entropy knot
  int n = eos_interped.size();
  vector<double> energy_knots, temp_knots;
  for (int i = 0; i < n; i++)
  {
    energy_knots.push_back(eos_interped[i].a);
    temp_knots.push_back(eos_interped[i].b);
  }
  //  Add the final knot (right endpoint of last spline interval)
  double ds_last = entropy.back() - eos_interped[n-1].x;
  energy_knots.push_back(energy.back());
  temp_knots.push_back(eos_interped[n-1].b + 2.0*eos_interped[n-1].c*ds_last + 3.0*eos_interped[n-1].d*ds_last*ds_last);

  temp_from_energy = CubicSpline(energy_knots, temp_knots);
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Romatschke-Strickland Lambda(energy density) integral helpers
//    epsilon(Lambda) = g * Integral d^3p  E_real * exp(-E_stretched/Lambda), Boltzmann statistics
//    E_real      = sqrt(pT^2 + pz^2 + m^2)          (actual on-shell energy, never stretched)
//    E_stretched = sqrt(pT^2 + (1+xi)*pz^2 + m^2)   (only the probability weight is squeezed)
//    Reduced analytically from 3D to 2D (pT, pz) using azimuthal symmetry and pz-parity;
//    the remaining double integral has no closed form once m != 0, so it's done with GSL.
//##########################################################################################
struct RSIntegralParams { double m; double Lambda; double xi; double pz; };

static double RS_pT_integrand(double pT, void *params)
{
  RSIntegralParams *p = (RSIntegralParams*)params;
  double E_real = sqrt(pT*pT + p->pz*p->pz + p->m*p->m);
  double E_stretched = sqrt(pT*pT + (1.0 + p->xi)*p->pz*p->pz + p->m*p->m);
  return pT * E_real * exp(-E_stretched / p->Lambda);
}

static double RS_pz_integrand(double pz, void *params)
{
  RSIntegralParams *p = (RSIntegralParams*)params;
  p->pz = pz;

  gsl_function F;
  F.function = &RS_pT_integrand;
  F.params = p;

  gsl_integration_workspace *w = gsl_integration_workspace_alloc(1000);
  double result, error;
  //  epsabs=1e-12 (not 0): once the integrand's decayed to ~0 (e.g. heavy quark, small Lambda),
  //  demanding pure relative precision on a value that's essentially zero is impossible and GSL
  //  aborts by default - an absolute floor lets it stop happily once the tail is truly negligible
  int status = gsl_integration_qagiu(&F, 0.0, 1e-12, 1e-7, 1000, w, &result, &error);
  gsl_integration_workspace_free(w);

  if (status != GSL_SUCCESS) { result = 0.0; }

  return result;
}

static double ComputeEpsilonOfLambda(double mass, double Lambda, double xi, double g)
{
  RSIntegralParams params = {mass, Lambda, xi, 0.0};

  gsl_function F;
  F.function = &RS_pz_integrand;
  F.params = &params;

  gsl_integration_workspace *w = gsl_integration_workspace_alloc(1000);
  double result, error;
  int status = gsl_integration_qagiu(&F, 0.0, 1e-12, 1e-7, 1000, w, &result, &error);
  gsl_integration_workspace_free(w);

  if (status != GSL_SUCCESS) { result = 0.0; }

  //  Factor of 2 from folding pz in (-inf,0] onto [0,inf) (integrand is even in pz),
  //  factor of 2*pi from the trivial azimuthal (phi) integral
  return g * 2.0 * 2.0 * M_PI * result;
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Build the 4 per-flavor Lambda(energy density) lookup tables, fixed xi=4, Boltzmann statistics
//  NOTE: flavor masses here must stay in sync with global.h's Charge struct (up/down/strange/charm)
//##########################################################################################
void IO::InitializeMomentumDistribution()
{
  //  don't let a single hard-to-reach-tolerance edge case (see RS_pz_integrand/ComputeEpsilonOfLambda)
  //  abort the whole program - those two functions already check the return status themselves
  gsl_set_error_handler_off();

  const double xi = 4.0;
  const double g = 6.0;              //  spin (2) x color (3) degeneracy for a single quark flavor
  const int n_lambda = 150;
  const double lambda_min = 0.02;    //  GeV
  const double lambda_max = 2.5;     //  GeV

  //  flavor_id=0 (mass=0) is the parent gluon itself - needed so its own momentum can be sampled
  //  once per pair and split via alpha, instead of sampling the quark and antiquark independently
  static const map<int,double> flavor_mass = { {0,0.0}, {1,0.0023}, {2,0.0048}, {3,0.095}, {4,1.29} };

  for (map<int,double>::const_iterator it = flavor_mass.begin(); it != flavor_mass.end(); ++it)
  {
    int flavor_id = it->first;
    double mass = it->second;

    vector<double> energy_density_grid, lambda_grid;

    for (int i = 0; i < n_lambda; i++)
    {
      //  Log-spaced so the low-energy-density tail (where Lambda changes fast) is well resolved
      double Lambda = lambda_min * pow(lambda_max/lambda_min, (double)i/(n_lambda-1));
      double eps = ComputeEpsilonOfLambda(mass, Lambda, xi, g);

      energy_density_grid.push_back(eps);
      lambda_grid.push_back(Lambda);
    }

    lambda_from_energy[flavor_id] = CubicSpline(energy_density_grid, lambda_grid);
  }
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Look up Lambda [GeV] for a given flavor and local energy density
//##########################################################################################
double IO::GetLambda(int flavor_id, double energy_density)
{
  SplineSet range = FindRange(lambda_from_energy[flavor_id], energy_density);
  return InterpolateValue(range, energy_density);
}
//__________________________________________________________________________________________
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Convert event input to energy
//##########################################################################################
void IO::ConvertEvent(vector<vector<double>> &input, double &total)
{
  SplineSet range;
  double entropy, energy;

  //  For each point in grid convert from entropy to energy
  for (int i = 0; i < input.size(); i++)
  {
    for (int j = 0; j < input[i].size(); j++)
    {
      //  If point is valued, do calculation
      if (input[i][j] > 0)
      {
        //  Multiply by scaling factor, this is because Trento uses thickness Functions
        //  which are not entropy but proportional to entropy
        entropy = a_trento*input[i][j];

        //  If entropy is above specified entropy cuttoff then convert to energy,
        //  otherwise set to 0 since these points won't be seen by hydro anyway
        if (entropy > s_chop)
        {
          //  Find range where entropy lies in eos
          range = FindRange(eos_interped, entropy);

          //  Make conversion from entropy to energy and add to total tracker
          input[i][j] = InterpolateValue(range, entropy);
          total += input[i][j];
        }
        else
        { input[i][j] = 0;  }
      }
    }
  }
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Print Density Grids with filler 0s
//##########################################################################################
void IO::OutputFullDensityGrids(vector<vector<double>> &density_grid, string file_name)
{
  //  Output file stream
  ofstream output;
  output.open(file_name);

  //******************************************************************************************
  //  Loop through density grids
  //******************************************************************************************
  for (int i = 0; i < density_grid.size(); i++)
  {
    for (int j = 0; j < density_grid[0].size(); j++)
    {

      output << density_grid[i][j];  //  Output value at grid point to file

      //  Test to see if at end of row
      if (j == density_grid[0].size() - 1)
      { output << endl;  }  //  If at end of row, go to next row
      else
      { output << " "; }  //  If not at end of row, add space
    }
  }

  output.close();
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Print Density Grids without filler 0s
//##########################################################################################
void IO::OutputSparseDensityGrids(vector<vector<double>> &density_grid, string file_name)
{
  ofstream output;
  output.open(file_name);

  double x, y, value;

  for (int i = 0; i < density_grid.size(); i++)
  {
    for (int j = 0; j < density_grid[0].size(); j++)
    {
      if (density_grid[i][j] != 0)
      {
        x = -grid_max + i*grid_step;  //  Converts grid point to physical x-value
        y = -grid_max + j*grid_step;  //  Converts grid point to physical y-value
        value = density_grid[i][j];
        output << x << " " << y << " " << value << endl;
      }
    }
  }

  output.close();
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
// Print Density Grids without filler 0s (Overload for output densities)
//##########################################################################################
void IO::OutputSparseDensityGrids(vector<vector<vector<double>>> &density_grid, double tot_energy, string file_name)
{
  ofstream output;
  output.open(file_name);

  output << current_event << " " << grid_step << " " << grid_step << " " << tot_energy << " " << -grid_max << " " << -grid_max << endl;

  double x, y;
  for (int i = 0; i < density_grid[0].size(); i++)
  {
    for (int j = 0; j < density_grid[0][0].size(); j++)
    {
      if (density_grid[0][i][j] != 0)
      {
        x = -grid_max + i*grid_step;  //  Converts grid point to physical x-value
        y = -grid_max + j*grid_step;  //  Converts grid point to physical y-value

        double e_cell = density_grid[0][i][j];
        double baryon  = density_grid[1][i][j];
        double strange = density_grid[2][i][j];

        //  Effective quark mass [GeV] from charge content of cell
        double m_eff;
        if (abs(strange) > 1e-10)
          m_eff = 0.095;   //  strange quark
        else if (abs(baryon) > 1e-10)
          m_eff = 0.0035;  //  light quark (avg up/down)
        else
          m_eff = 0.0;     //  gluon-dominated cell

        //  Velocity: v = sqrt(1 - m^2/E^2), E_mean = 2.7*T (thermal average)
        double velocity;
        if (m_eff < 1e-15)
        {
          velocity = 1.0;  //  massless -> v = c
        }
        else
        {
          SplineSet range = FindRange(temp_from_energy, e_cell);
          double T = InterpolateValue(range, e_cell);
          double E_mean = 2.7 * T;
          double ratio = m_eff / E_mean;
          velocity = (ratio < 1.0) ? sqrt(1.0 - ratio * ratio) : 0.0;
        }

        output << x << " " << y << " "
        << density_grid[0][i][j] << " "
        << density_grid[1][i][j] << " "
        << density_grid[2][i][j] << " "
        << density_grid[3][i][j] << " "
        << velocity << endl;
      }
    }
  }

  output.close();
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Print Eccentricities
//##########################################################################################
void IO::OutputEccentricities(double total_entropy, vector<vector<double>> eccentricities, string density_type, string file_name)
{
  //  If outputing energy eccentricities, run these commands
  if (density_type == "Energy")
  {
    ofstream output;
    output.open(file_name + ".dat", ios::app);  //  Append event to end of file

    //  Print event number and total entropy
    output << current_event << " " << total_entropy << " ";

    //  Print magnitude and angle of eccentricities
    for (int i = 0; i < eccentricities.size(); i++)
    {
      output << eccentricities[i][0] << " " << eccentricities[i][1] << " ";
    }

    //  Output radius
    output << eccentricities[0][2] << endl;

    output.close();
  }
  //  If outputing charge eccentricities, print negative and positive to different files
  else if (density_type == "Charge")
  {
    ofstream output_pos, output_neg;
    output_neg.open(file_name + "_neg.dat", ios::app);  //  Append event to end of file
    output_pos.open(file_name + "_pos.dat", ios::app);  //  Append event to end of file

    //  Print event number and total entropy
    output_neg << current_event << " " << total_entropy << " ";
    output_pos << current_event << " " << total_entropy << " ";

    //  Print magnitude and angle of eccentricities
    for (int i = 0; i < eccentricities.size(); i++)
    {
      output_neg << eccentricities[i][0] << " " << eccentricities[i][1] << " ";
      output_pos << eccentricities[i][3] << " " << eccentricities[i][4] << " ";
    }

    //  Output radius
    output_neg << eccentricities[0][2] << endl;
    output_pos << eccentricities[0][5] << endl;

    output_neg.close();
    output_pos.close();
  }
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Print quark counts
//##########################################################################################
void IO::OutputQuarkCounts(double total_entropy, int gluon, int up, int down, int strange, int charm, string file_name)
{
  ofstream output;
  output.open(file_name, ios::app);  //  Append event to end of file

  output << current_event << " " << total_entropy << " " << gluon << " " << up << " " << down << " " << strange << " " << charm << endl;
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Samples one (px,py,pz) from the Romatschke-Strickland distribution (Boltzmann stats, given
//  xi) for a particle of the given mass and Lambda.
//    Trick: f_RS(px,py,pz) is exactly an isotropic thermal Boltzmann distribution in the
//    "primed" variable pz'=sqrt(1+xi)*pz. So: sample p'=sqrt(px^2+py^2+pz'^2) isotropically
//    (rejection sampling against the radial thermal weight p'^2*exp(-E'/Lambda)), sample a
//    random direction, then un-stretch pz = pz'/sqrt(1+xi) at the very end. px,py are untouched.
//##########################################################################################
static void SampleThermalMomentum(double mass, double Lambda, double xi, double &px, double &py, double &pz)
{
  //  Peak of p'^2*exp(-sqrt(p'^2+m^2)/Lambda), used to build a rejection-sampling envelope
  double p_peak = sqrt(2.0*Lambda*(Lambda + sqrt(Lambda*Lambda + mass*mass)));
  double E_peak = sqrt(p_peak*p_peak + mass*mass);
  double f_max = p_peak*p_peak * exp(-E_peak/Lambda);
  double p_cut = p_peak + 30.0*Lambda;  //  generous cutoff, exponential tail is negligible past this

  uniform_real_distribution<double> get_p(0.0, p_cut);
  uniform_real_distribution<double> get_reject(0.0, f_max);
  uniform_real_distribution<double> get_costheta(-1.0, 1.0);
  uniform_real_distribution<double> get_phi(0.0, 2.0*M_PI);

  double p_prime = 0.0;
  bool accepted = false;
  while (!accepted)
  {
    double p_candidate = get_p(get_random_number);
    double E_candidate = sqrt(p_candidate*p_candidate + mass*mass);
    double f_val = p_candidate*p_candidate * exp(-E_candidate/Lambda);

    if (get_reject(get_random_number) <= f_val)
    {
      p_prime = p_candidate;
      accepted = true;
    }
  }

  double costheta = get_costheta(get_random_number);
  double sintheta = sqrt(1.0 - costheta*costheta);
  double phi = get_phi(get_random_number);

  double pT = p_prime * sintheta;
  px = pT * cos(phi);
  py = pT * sin(phi);
  double pz_prime = p_prime * costheta;

  pz = pz_prime / sqrt(1.0 + xi);  //  un-stretch back to the real beam-direction momentum
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Samples ONE momentum for the parent gluon (flavor_id=0, mass=0) per pair, at its own
//  pre-split position, then splits it via alpha between the quark (row i) and antiquark
//  (row i+1) - so their momenta actually add back up to the gluon's, unlike sampling them
//  independently. Must run after the event's density[0] grid is fully built (i.e. after
//  CalculateEccentricities), not mid-event, since it needs the true local energy density.
//##########################################################################################
void IO::AssignPairMomentum(vector<vector<double>> &quark_list, vector<vector<double>> &energy_density_grid)
{
  const double xi = 4.0;

  for (int i = 0; i < quark_list.size(); i += 2)
  {
    //  gluon_x/gluon_y/alpha (columns 8,9,10) are identical on the quark and antiquark row
    int gluon_x = (int)quark_list[i][8];
    int gluon_y = (int)quark_list[i][9];
    double alpha = quark_list[i][10];

    double e_local = energy_density_grid[gluon_x][gluon_y];

    //  Defensive clamp: the gluon center can sit where the local cell value is below the
    //  lowest energy density the Lambda table was built over
    double e_min = lambda_from_energy[0].front().x;
    if (e_local < e_min) { e_local = e_min; }

    double Lambda_gluon = GetLambda(0, e_local);

    double px_g, py_g, pz_g;
    SampleThermalMomentum(0.0, Lambda_gluon, xi, px_g, py_g, pz_g);

    //  Quark gets alpha's share, antiquark gets (1-alpha)'s share - same convention already
    //  used for splitting the gluon's energy between them (Event::UpdateDensity)
    double mass = quark_list[i][3];  //  same flavor mass on both rows

    double px_q = alpha*px_g,       py_q = alpha*py_g,       pz_q = alpha*pz_g;
    double px_a = (1.0-alpha)*px_g, py_a = (1.0-alpha)*py_g, pz_a = (1.0-alpha)*pz_g;

    double E_q = sqrt(px_q*px_q + py_q*py_q + pz_q*pz_q + mass*mass);
    double E_a = sqrt(px_a*px_a + py_a*py_a + pz_a*pz_a + mass*mass);

    quark_list[i].push_back(Lambda_gluon);
    quark_list[i].push_back(px_q); quark_list[i].push_back(py_q); quark_list[i].push_back(pz_q);
    quark_list[i].push_back(px_q/E_q); quark_list[i].push_back(py_q/E_q); quark_list[i].push_back(pz_q/E_q);

    quark_list[i+1].push_back(Lambda_gluon);
    quark_list[i+1].push_back(px_a); quark_list[i+1].push_back(py_a); quark_list[i+1].push_back(pz_a);
    quark_list[i+1].push_back(px_a/E_a); quark_list[i+1].push_back(py_a/E_a); quark_list[i+1].push_back(pz_a/E_a);
  }
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Print per-quark position, flavor, mass, charges, RS Lambda, momentum, and velocity
//##########################################################################################
void IO::OutputQuarkList(vector<vector<double>> &quark_list, string file_name)
{
  ofstream output;
  output.open(file_name, ios::app);  //  Append event to end of file

  string flavor_label;
  double x, y, gluon_x, gluon_y;

  for (int i = 0; i < quark_list.size(); i++)
  {
    x = -grid_max + quark_list[i][0]*grid_step;  //  Converts grid point to physical x-value
    y = -grid_max + quark_list[i][1]*grid_step;  //  Converts grid point to physical y-value
    gluon_x = -grid_max + quark_list[i][8]*grid_step;  //  Parent gluon's pre-split physical x-value
    gluon_y = -grid_max + quark_list[i][9]*grid_step;  //  Parent gluon's pre-split physical y-value

    if (quark_list[i][2] == 1) { flavor_label = "up"; }
    else if (quark_list[i][2] == 2) { flavor_label = "down"; }
    else if (quark_list[i][2] == 3) { flavor_label = "strange"; }
    else if (quark_list[i][2] == 4) { flavor_label = "charm"; }

    output << current_event << " " << x << " " << y << " " << flavor_label << " "
           << quark_list[i][3] << " " << quark_list[i][4] << " " << quark_list[i][5] << " "
           << quark_list[i][6] << " " << quark_list[i][7] << " "
           << gluon_x << " " << gluon_y << " " << quark_list[i][10] << " " << quark_list[i][11] << " "
           << quark_list[i][12] << " " << quark_list[i][13] << " " << quark_list[i][14] << " "
           << quark_list[i][15] << " " << quark_list[i][16] << " " << quark_list[i][17] << endl;
  }

  output.close();
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Read Event specific Energy densities
//    Create Event and set densities and variables
//    (Assumes sparse file with only valued points, need to generalize this)
//##########################################################################################
Event IO::ReadEvent(Event event_in)
{
  //  Input file stream
  ifstream input;
  input.open(trento_input_dir + "ic" + to_string(current_event) + ".dat");

  // Loop input variables
  int x = 0, y = 0;
  double readx,ready,value,numpoints=0;

  //  Ignore first line of input file
  input.ignore(10000, '\n');

  //******************************************************************************************
  //  Loop through file until end is reached
  //    If input_type = 0, then read in file as full grid with filler 0's, else
  //    if input_type = 1, then read in file as sparse (only valued points) format
  //******************************************************************************************
  if (input_type == 0)
  {
    string line;

    //  Loop through file and read line by line for full grid
    while (getline(input, line))
    {
        istringstream event_line(line);
        //  Seperate each value in line and set corresponding coordinate
        //  in initial_energy to value while updating other important variables
        while(event_line >> value)
        {
          event_in.valued_points.push_back({{x},{y}});

          event_in.initial_energy[x][y] = value;
          event_in.total_initial_entropy += value;
          if(value > 0) { numpoints++;  }

          // increment y (column value)
          y++;
        }

        //  increment x (row value)
        x++;
      }
  }
  else if (input_type == 1)
  {
    while (!input.eof())
    {
        //  Read in point from energy density
        input >> readx >> ready >> value;

        //  Take physical point and convert x and y values into grid indicies
        x = (int)round((readx + grid_max)/grid_step);
        y = (int)round((ready + grid_max)/grid_step);
        event_in.valued_points.push_back({{x},{y}});

        //  Set point in event's initial energy density grid
        event_in.initial_energy[x][y] = value;
        event_in.total_initial_entropy += value;
        numpoints++;

        input.ignore(10000, '\n');  //  Ignore rest of line
        if (input.peek() == '\n') {break;}  //  Saftey check for empty line at end of file

      }
    }
  input.close();  //  Close input stream


  ConvertEvent(event_in.initial_energy, event_in.total_initial_energy);
  event_in.total_initial_entropy = a_trento*event_in.total_initial_entropy/numpoints;

  //******************************************************************************************
  //  If method requires T_a energy density, read it into event
  //******************************************************************************************
  if (t_a)
  {
    //  Open T_a energy density file
    input.open(trento_input_dir + "TA" + to_string(current_event) + ".dat");
    //  Initialize t_a grid to 0 with dimensions grid_points + 1
    event_in.t_a.resize(grid_points + 1, vector<double>(grid_points + 1, 0));

    //  Ignore first line of input file
    input.ignore(10000, '\n');

    //  Loop through file until end is reached
    if (input_type == 0)
    {
      string line;

      //  Loop through file and read line by line for full grid
      while (getline(input, line))
      {
          istringstream event_line(line);
          //  Seperate each value in line and set corresponding coordinate
          //  in t_a to value while updating other important variables
          while(event_line >> value)
          {
            event_in.t_a[x][y] = value;

            y++;  // increment y (column value)
          }
          x++;  //  increment x (row value)
        }
    }
    else if (input_type == 1)
    {
      while (!input.eof())
      {
          //  Read in point from energy density
          input >> readx >> ready >> value;

          //  Take physical point and convert x and y values into grid indicies
          x = (int)round((readx + grid_max)/grid_step);
          y = (int)round((ready + grid_max)/grid_step);

          //  Set point in event's initial energy density grid
          event_in.t_a[x][y] = value;

          input.ignore(10000, '\n');  //  Ignore rest of line
          if (input.peek() == '\n') {break;}  //  Saftey check for empty line at end of file
        }
      }
    input.close();  //  Close input stream
  }

  //******************************************************************************************
  //  If method requires T_b energy density, read it into event
  //******************************************************************************************
  if (t_b)
  {
    //  Open T_a energy density file
    input.open(trento_input_dir + "TB" + to_string(current_event) + ".dat");
    //  Initialize t_b grid to 0 with dimensions grid_points + 1
    event_in.t_b.resize(grid_points + 1, vector<double>(grid_points + 1, 0));

    //  Ignore first line of input file
    input.ignore(10000, '\n');

    //  Loop through file until end is reached
    if (input_type == 0)
    {
      string line;

      //  Loop through file and read line by line for full grid
      while (getline(input, line))
      {
          istringstream event_line(line);
          //  Seperate each value in line and set corresponding coordinate
          //  in t_b to value while updating other important variables
          while(event_line >> value)
          {
            event_in.t_b[x][y] = value;

            y++;  // increment y (column value)
          }
          x++;  //  increment x (row value)
        }
    }
    else if (input_type == 1)
    {
      while (!input.eof())
      {
        //  Read in point from energy density
        input >> readx >> ready >> value;
        //  Take physical point and convert x and y values into grid indicies
        x = (int)round((readx + grid_max)/grid_step);
        y = (int)round((ready + grid_max)/grid_step);

        //  Set point in event's initial energy density grid
        event_in.t_b[x][y] = value;
        input.ignore(10000, '\n');  //  Ignore rest of line
        if (input.peek() == '\n') {break;}  //  Saftey check for empty line at end of file
      }
    }
    input.close();  //  Close input stream
  }

  return event_in;  //  Return event with data
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Write Energy densities, eccentricities, and configFile
//    Needs to be updated as code is being written
//##########################################################################################
void IO::WriteEvent(Event event)
{
  //******************************************************************************************
  //  Output Full Density Grids
  //******************************************************************************************
  if (output_type == 0)
  {
    OutputFullDensityGrids(event.density[0], output_dir + "energy_density_" + to_string(current_event) + ".dat");
    OutputFullDensityGrids(event.density[1], output_dir + "baryon_density_" + to_string(current_event) + ".dat");
    OutputFullDensityGrids(event.density[2], output_dir + "strange_density_" + to_string(current_event) + ".dat");
    OutputFullDensityGrids(event.density[3], output_dir + "charge_density_" + to_string(current_event) + ".dat");

    if (t_a)  //  Output T_a if flag is true
    {
      OutputFullDensityGrids(event.t_a, output_dir + "ta" + to_string(current_event) + ".dat");
    }
    if (t_b)  //  Output T_b if flag is true
    {
      OutputFullDensityGrids(event.t_b, output_dir + "tb" + to_string(current_event) + ".dat");
    }
  }
  //******************************************************************************************
  //  Output Sparse Density Grids
  //******************************************************************************************
  else if (output_type == 1)
  {
    OutputSparseDensityGrids(event.density, event.total_energy, output_dir + "densities" + to_string(current_event) + ".dat");

    if (t_a)  //  Output T_a if flag is true
    {
      OutputSparseDensityGrids(event.t_a, output_dir + "ta" + to_string(current_event) + ".dat");
    }
    if (t_b)  //  Output T_b if flag is true
    {
      OutputSparseDensityGrids(event.t_b, output_dir + "ta" + to_string(current_event) + ".dat");
    }
  }

  OutputEccentricities(event.total_initial_entropy, event.eccentricities[0], "Energy", output_dir + "energy_eccentricities");
  OutputEccentricities(event.total_initial_entropy, event.eccentricities[1], "Charge", output_dir + "baryon_eccentricities");
  OutputEccentricities(event.total_initial_entropy, event.eccentricities[2], "Charge", output_dir + "strange_eccentricities");
  OutputEccentricities(event.total_initial_entropy, event.eccentricities[3], "Charge", output_dir + "charge_eccentricities");
  OutputQuarkCounts(event.total_initial_entropy, event.number_gluon, event.number_up, event.number_down, event.number_strange, event.number_charm, output_dir + "quark_counts.dat");
  AssignPairMomentum(event.quark_list, event.density[0]);
  OutputQuarkList(event.quark_list, output_dir + "quark_list.dat");
  current_event++;  //  Used for tracking which event has been processed
}
//__________________________________________________________________________________________

//__________________________________________________________________________________________
//##########################################################################################
//  Return true if on last event
//##########################################################################################
bool IO::LastEvent()
{
  if (current_event > last_event)
  { return true;  }
  else
  { return false; }
}
//__________________________________________________________________________________________
