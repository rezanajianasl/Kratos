//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Joaquin Gonzalez-Usua
//
//

// System includes

#include <math.h>
// External includes

// Project includes
#include "utilities/openmp_utils.h"
#include "containers/model.h"
#include "includes/checks.h"
#include "utilities/variable_utils.h"

// Application includes
#include "swimming_DEM_application.h"
#include "spatial_dependant_porosity_solution_body_force_process.h"
#include "swimming_dem_application_variables.h"

namespace Kratos
{

/* Public functions *******************************************************/
SpatialDependantPorositySolutionBodyForceProcess::SpatialDependantPorositySolutionBodyForceProcess(
    ModelPart& rModelPart,
    const double Density,
    const double Viscosity,
    const double IndependentTerm,
    const double MaximumAlpha,
    const double Centerx1,
    const double Centerx2)
    : Process(),
      mrModelPart(rModelPart)
{
    // Member variables initialization
    mDensity         = Density;
    mViscosity       = Viscosity;
    mIndependentTerm = IndependentTerm;
    mMaximumAlpha    = MaximumAlpha;
    mCenterx1        = Centerx1;
    mCenterx2        = Centerx2;

}

SpatialDependantPorositySolutionBodyForceProcess::SpatialDependantPorositySolutionBodyForceProcess(
    ModelPart& rModelPart,
    Parameters& rParameters)
    : Process(),
      mrModelPart(rModelPart)
{
    // Check default settings
    this->CheckDefaultsAndProcessSettings(rParameters);
}

SpatialDependantPorositySolutionBodyForceProcess::SpatialDependantPorositySolutionBodyForceProcess(
    Model &rModel,
    Parameters &rParameters)
    : Process(),
      mrModelPart(rModel.GetModelPart(rParameters["model_part_name"].GetString()))
{
    // Check default settings
    this->CheckDefaultsAndProcessSettings(rParameters);

}


void SpatialDependantPorositySolutionBodyForceProcess::CheckDefaultsAndProcessSettings(Parameters &rParameters)
{
    Parameters default_parameters( R"(
    {
                "model_part_name"          : "please_specify_model_part_name",
                "variable_name"            : "BODY_FORCE",
                "benchmark_name"           : "custom_body_force.vortex",
                "benchmark_parameters"     : {
                                                "velocity"    : 1.0,
                                                "length"      : 1.0,
                                                "viscosity"   : 0.1,
                                                "density"     : 1.0,
                                                "frequency"   : 1.0,
                                                "damping"     : 1.0,
                                                "independent_term"  : 0.4,
                                                "maximum_alpha"     : 1.0,
                                                "center_x1"   : 0.0,
                                                "center_x2"   : 0.0
                },
                "compute_nodal_error"      : true,
                "print_convergence_output" : false,
                "output_parameters"        : {}
    }  )" );

    rParameters.ValidateAndAssignDefaults(default_parameters);

    mDensity   = rParameters["benchmark_parameters"]["density"].GetDouble();
    mViscosity = rParameters["benchmark_parameters"]["viscosity"].GetDouble();
    mIndependentTerm = rParameters["benchmark_parameters"]["independent_term"].GetDouble();
    mMaximumAlpha    = rParameters["benchmark_parameters"]["maximum_alpha"].GetDouble();
    mCenterx1  = rParameters["benchmark_parameters"]["center_x1"].GetDouble();
    mCenterx2  = rParameters["benchmark_parameters"]["center_x2"].GetDouble();
}

void SpatialDependantPorositySolutionBodyForceProcess::Execute()
{
    this->ExecuteInitialize();
    this->ExecuteInitializeSolutionStep();
}

void SpatialDependantPorositySolutionBodyForceProcess::ExecuteInitialize()
{}

void SpatialDependantPorositySolutionBodyForceProcess::ExecuteBeforeSolutionLoop() 
{
    this->SetInitialBodyForceAndPorosityField();
}

void SpatialDependantPorositySolutionBodyForceProcess::ExecuteInitializeSolutionStep() 
{
    this->SetBodyForceAndPorosityField();
}

void SpatialDependantPorositySolutionBodyForceProcess::ExecuteFinalizeSolutionStep() {}

/* Protected functions ****************************************************/

void SpatialDependantPorositySolutionBodyForceProcess::SetInitialBodyForceAndPorosityField() {

    const double time = mrModelPart.GetProcessInfo()[TIME];
    const double maximum_alpha = mMaximumAlpha;
    const double centerx1 = mCenterx1;
    const double centerx2 = mCenterx2;
    const double independent_term = mIndependentTerm;
    const double rho = mDensity;
    const double nu = mViscosity;

    // BodyForce and Porosity fields at time 0.0
    #pragma omp parallel for
    for (int i_node = 1; i_node <= static_cast<int>(mrModelPart.NumberOfNodes()); ++i_node){
            int it_node = i_node;
            const double x1 = mrModelPart.GetNode(it_node).X();
            const double x2 = mrModelPart.GetNode(it_node).Y();

            const double alpha = -independent_term * x1 - independent_term * x2 + maximum_alpha;;

            const double alpha1 = -independent_term;

            const double alpha2 = -independent_term;

            const double u1 = 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double u2 = 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du1dt = -100*M_PI*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*sin(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) - 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du2dt = -100*M_PI*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*sin(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) - 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du11 = 100*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du12 = 100*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(200*(-2*centerx2 + 2*x2)*(-2*centerx2 + 2*x2 - 2) + 200*pow(-centerx2 + x2, 2) + 200*pow(centerx2 - x2 + 1, 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du122 = 200*pow(independent_term,2)*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(200*(-2*centerx2 + 2*x2)*(-2*centerx2 + 2*x2 - 2) + 200*pow(-centerx2 + x2, 2) + 200*pow(centerx2 - x2 + 1, 2))*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(-2400*centerx2 + 2400*x2 - 1200)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du111 = 200*pow(independent_term,2)*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du22 = 100*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du222 = 200*pow(independent_term,2)*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du21 = 100*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx2 + x2, 2)*pow(centerx2 - x2 + 1, 2)*(-200*(-2*centerx1 + 2*x1)*(-2*centerx1 + 2*x1 - 2) - 200*pow(-centerx1 + x1, 2) - 200*pow(centerx1 - x1 + 1, 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du211 = 200*pow(independent_term,2)*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*pow(-centerx2 + x2, 2)*pow(centerx2 - x2 + 1, 2)*(-200*(-2*centerx1 + 2*x1)*(-2*centerx1 + 2*x1 - 2) - 200*pow(-centerx1 + x1, 2) - 200*pow(centerx1 - x1 + 1, 2))*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx2 + x2, 2)*(2400*centerx1 - 2400*x1 + 1200)*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double convective1 = u1 * du11 + u2 * du12; 
            const double convective2 = u1 * du21 + u2 * du22;
            
            const double laplacian1 = du111 + du122;
            const double laplacian2 = du211 + du222;

            const double press_grad1 = 0.0;
            const double press_grad2 = 0.0;

            const double body_force1 = du1dt + convective1 + 1/rho * press_grad1 - nu * laplacian1;
            const double body_force2 = du2dt + convective2 + 1/rho * press_grad2 - nu * laplacian2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(VELOCITY_X) = u1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(VELOCITY_Y) = u2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION) = alpha;
            
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION_GRADIENT_X) = alpha1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION_GRADIENT_Y) = alpha2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(BODY_FORCE_X) = body_force1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(BODY_FORCE_Y) = body_force2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(EXACT_VELOCITY_X) = u1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(EXACT_VELOCITY_Y) = u2;
        }

}

void SpatialDependantPorositySolutionBodyForceProcess::SetBodyForceAndPorosityField() {

    const double time = mrModelPart.GetProcessInfo()[TIME];
    const double maximum_alpha = mMaximumAlpha;
    const double centerx1 = mCenterx1;
    const double centerx2 = mCenterx2;
    const double independent_term = mIndependentTerm;
    const double rho = mDensity;
    const double nu = mViscosity;

    // Computation of the BodyForce and Porosity fields
    #pragma omp parallel for
    for (int i_node = 1; i_node <= static_cast<int>(mrModelPart.NumberOfNodes()); ++i_node){
            int it_node = i_node;
            const double x1 = mrModelPart.GetNode(it_node).X();
            const double x2 = mrModelPart.GetNode(it_node).Y();

            const double alpha = -independent_term * x1 - independent_term * x2 + maximum_alpha;

            const double alpha1 = -independent_term;

            const double alpha2 = -independent_term;

            const double u1 = 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double u2 = 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du1dt = -100*M_PI*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*sin(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) - 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du2dt = -100*M_PI*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*sin(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) - 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du11 = 100*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 100*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du12 = 100*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(200*(-2*centerx2 + 2*x2)*(-2*centerx2 + 2*x2 - 2) + 200*pow(-centerx2 + x2, 2) + 200*pow(centerx2 - x2 + 1, 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du122 = 200*pow(independent_term,2)*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(200*(-2*centerx2 + 2*x2)*(-2*centerx2 + 2*x2 - 2) + 200*pow(-centerx2 + x2, 2) + 200*pow(centerx2 - x2 + 1, 2))*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx1 + x1, 2)*pow(centerx1 - x1 + 1, 2)*(-2400*centerx2 + 2400*x2 - 1200)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du111 = 200*pow(independent_term,2)*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*independent_term*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*(-2*centerx1 + 2*x1)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*(-2*centerx1 + 2*x1 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*pow(-centerx1 + x1, 2)*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*(100*(-2*centerx2 + 2*x2)*pow(centerx2 - x2 + 1, 2) + 100*pow(-centerx2 + x2, 2)*(-2*centerx2 + 2*x2 - 2))*pow(centerx1 - x1 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du22 = 100*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 100*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du222 = 200*pow(independent_term,2)*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 200*(-2*centerx2 + 2*x2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*(-2*centerx2 + 2*x2 - 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha) + 200*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du21 = 100*independent_term*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx2 + x2, 2)*pow(centerx2 - x2 + 1, 2)*(-200*(-2*centerx1 + 2*x1)*(-2*centerx1 + 2*x1 - 2) - 200*pow(-centerx1 + x1, 2) - 200*pow(centerx1 - x1 + 1, 2))*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double du211 = 200*pow(independent_term,2)*pow(-centerx2 + x2, 2)*(-100*(-2*centerx1 + 2*x1)*pow(centerx1 - x1 + 1, 2) - 100*pow(-centerx1 + x1, 2)*(-2*centerx1 + 2*x1 - 2))*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 3) + 200*independent_term*pow(-centerx2 + x2, 2)*pow(centerx2 - x2 + 1, 2)*(-200*(-2*centerx1 + 2*x1)*(-2*centerx1 + 2*x1 - 2) - 200*pow(-centerx1 + x1, 2) - 200*pow(centerx1 - x1 + 1, 2))*exp(-time)*cos(M_PI*time)/pow(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha, 2) + 100*pow(-centerx2 + x2, 2)*(2400*centerx1 - 2400*x1 + 1200)*pow(centerx2 - x2 + 1, 2)*exp(-time)*cos(M_PI*time)/(-independent_term*(-centerx1 + x1) - independent_term*(-centerx2 + x2) + maximum_alpha);

            const double convective1 = u1 * du11 + u2 * du12; 
            const double convective2 = u1 * du21 + u2 * du22;
            
            const double laplacian1 = du111 + du122;
            const double laplacian2 = du211 + du222;

            const double press_grad1 = 0.0;
            const double press_grad2 = 0.0;

            const double body_force1 = du1dt + convective1 + 1/rho * press_grad1 - nu * laplacian1;
            const double body_force2 = du2dt + convective2 + 1/rho * press_grad2 - nu * laplacian2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION) = alpha;
            
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION_GRADIENT_X) = alpha1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(FLUID_FRACTION_GRADIENT_Y) = alpha2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(BODY_FORCE_X) = body_force1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(BODY_FORCE_Y) = body_force2;

            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(EXACT_VELOCITY_X) = u1;
            mrModelPart.GetNode(it_node).FastGetSolutionStepValue(EXACT_VELOCITY_Y) = u2;
        }

}



/* Private functions ****************************************************/

};  // namespace Kratos.