//  KRATOS  \   |  |   |  |  __  __| _ _|      
//         |\ / |  |   |  |      |     |       
//        _|  _|  \___/  _____| _|   ___| SCALE
//
#include "multiscale_application_variables.h"

namespace Kratos
{

KRATOS_CREATE_VARIABLE(Vector, INITIAL_TEMP_GRAD)
	
KRATOS_CREATE_VARIABLE(std::string, RVE_CLAW_MAP_NAME)
//KRATOS_CREATE_VARIABLE(TagStrainVectorMap, RVE_CLAW_MAP)
KRATOS_CREATE_VARIABLE(Vector, HEAT_FLUX_RVE)
KRATOS_CREATE_VARIABLE(Vector, FLUX_RVE)
KRATOS_CREATE_VARIABLE(double, HOMOGENIZED_CONDUCTIVITY)
KRATOS_CREATE_VARIABLE(double, HOMOGENIZED_CTE)
KRATOS_CREATE_VARIABLE(Matrix, HOMOGENIZED_CONST_TENS)
KRATOS_CREATE_VARIABLE(Matrix, INVERSE_HOMOGENIZED_CONST_TENS)
KRATOS_CREATE_VARIABLE(double, RVE_NON_LINEAR_FLAG)
KRATOS_CREATE_VARIABLE(Vector, ACTUAL_TAG)
KRATOS_CREATE_VARIABLE(Matrix, RVE_GENERAL_STRESS_TENSOR)
KRATOS_CREATE_VARIABLE(double, EQUIVALENT_DAMAGE)
KRATOS_CREATE_VARIABLE(double, CLAW_LIMIT_DAMAGE)
KRATOS_CREATE_VARIABLE(Matrix, PREDICTED_STRESS_TENSOR)

KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_1111)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_1122)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_1133)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_2211)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_2222)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_2233)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_3311)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_3322)
KRATOS_CREATE_VARIABLE(double, CONDUCTIVITY_3333)

KRATOS_CREATE_VARIABLE(double, ASPERITIES_CONSTANT)
KRATOS_CREATE_VARIABLE(double, BETA_THERMAL_COEFF)
KRATOS_CREATE_VARIABLE(double, YOUNG_MAT1)
KRATOS_CREATE_VARIABLE(double, CONDUCT_MAT1)
KRATOS_CREATE_VARIABLE(double, POISSON_MAT1)
KRATOS_CREATE_VARIABLE(double, YOUNG_MAT2)
KRATOS_CREATE_VARIABLE(double, CONDUCT_MAT2)
KRATOS_CREATE_VARIABLE(double, POISSON_MAT2)
KRATOS_CREATE_VARIABLE(double, AIR_CONDUCTIVITY)

// for rve
KRATOS_CREATE_VARIABLE(int, RVE_CONSTITUTIVE_LAW_FLAG)
KRATOS_CREATE_VARIABLE(int, RVE_PREDICTION_FLAG)
KRATOS_CREATE_VARIABLE(int, RVE_DAMAGE_SURFACE_FLAG)
KRATOS_CREATE_VARIABLE(Vector, RVE_DAMAGE_SURFACE_LIMIT)
KRATOS_CREATE_VARIABLE(double, RVE_FULL_TEMPERATURE)
KRATOS_CREATE_VARIABLE(double, TEMPERATURE_REACTION)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( RVE_FULL_DISPLACEMENT )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( RVE_WPC_LAGRANGIAN_DOF )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( RVE_WPC_LAGRANGIAN_REACTION )
KRATOS_CREATE_VARIABLE( double, RVE_WPR_LAGRANGIAN_DOF )
KRATOS_CREATE_VARIABLE( double, RVE_WPR_LAGRANGIAN_REACTION )

// for lagrange multipliers
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( DISPLACEMENT_LAGRANGE )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( ROTATION_LAGRANGE )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( REACTION_DISPLACEMENT_LAGRANGE )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( REACTION_ROTATION_LAGRANGE )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( DISPLACEMENT_DOUBLE_LAGRANGE_1 )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( DISPLACEMENT_DOUBLE_LAGRANGE_2 )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( ROTATION_DOUBLE_LAGRANGE_1 )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( ROTATION_DOUBLE_LAGRANGE_2 )
KRATOS_CREATE_VARIABLE( double, DOUBLE_LAGRANGE_SCALE_FACTOR_ALPHA )
KRATOS_CREATE_VARIABLE( double, DOUBLE_LAGRANGE_SCALE_FACTOR_BETA )

// for strategies
KRATOS_CREATE_VARIABLE( int, STRATEGY_SOLUTION_STEP_SOLVED )
KRATOS_CREATE_VARIABLE( int, STRATEGY_FINALIZE_SOLUTION_STEP_LEVEL )
KRATOS_CREATE_VARIABLE( double, CONSTITUTIVE_INTEGRATION_ERROR_CODE )
KRATOS_CREATE_VARIABLE( int, ITERATION_CONVERGENCE_FLAG )
KRATOS_CREATE_VARIABLE( double, SUGGESTED_TIME_STEP )

// for damage constitutive law
KRATOS_CREATE_VARIABLE( Vector, GAP_INTERFACE )
KRATOS_CREATE_VARIABLE( double, CONVECTION_DEGRADATION )
KRATOS_CREATE_VARIABLE( int, EXPONENTIAL_DAMAGE )

KRATOS_CREATE_VARIABLE( double, DAMAGE_T_INTERFACE )
KRATOS_CREATE_VARIABLE( double, DAMAGE_C_INTERFACE )
KRATOS_CREATE_VARIABLE(double, DAMAGE_T)
KRATOS_CREATE_VARIABLE(double, DAMAGE_C)
KRATOS_CREATE_VARIABLE( double, FRACTURE_ENERGY_T )
KRATOS_CREATE_VARIABLE( double, FRACTURE_ENERGY_C )
KRATOS_CREATE_VARIABLE( double, YIELD_STRESS_T )
KRATOS_CREATE_VARIABLE( double, YIELD_STRESS_C )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRESS_T_0 )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRESS_C_0 )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRESS_C_P )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRESS_C_M )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRESS_C_R )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRAIN_C_P )
KRATOS_CREATE_VARIABLE( double, DAMAGE_STRAIN_C_M )
KRATOS_CREATE_VARIABLE( double, DAMAGE_COMPRESSIVE_LAW_C1 )
KRATOS_CREATE_VARIABLE( double, DAMAGE_COMPRESSIVE_LAW_C2 )
KRATOS_CREATE_VARIABLE( double, DAMAGE_COMPRESSIVE_LAW_C3 )
KRATOS_CREATE_VARIABLE( double, BIAXIAL_COMPRESSION_MULTIPLIER )
KRATOS_CREATE_VARIABLE( double, SHEAR_COMPRESSION_REDUCTION )
KRATOS_CREATE_VARIABLE( double, DAMAGE_TENSILE_SURFACE_S1 )
KRATOS_CREATE_VARIABLE( double, LUBLINER_SURFACE_PARAM_KC )
KRATOS_CREATE_VARIABLE( double, GENRANKINE_SURFACE_PARAM_A )
KRATOS_CREATE_VARIABLE( double, GENRANKINE_SURFACE_PARAM_B )
KRATOS_CREATE_VARIABLE( double, GENRANKINE_SURFACE_PARAM_C )
KRATOS_CREATE_VARIABLE( int, DAMAGE_SECANT_MATRIX )
KRATOS_CREATE_VARIABLE( int, DAMAGE_MODEL )
KRATOS_CREATE_VARIABLE( int, DAMAGE_TENSILE_MODEL )

// for interface constitutive law
KRATOS_CREATE_VARIABLE( double, NORMAL_STIFFNESS )
KRATOS_CREATE_VARIABLE( double, TANGENTIAL_STIFFNESS )
KRATOS_CREATE_VARIABLE( double, NORMAL_STIFFNESS_COMPRESSION_MULTIPLIER )
KRATOS_CREATE_VARIABLE( double, FRACTURE_ENERGY_MODE_I )
KRATOS_CREATE_VARIABLE( double, FRACTURE_ENERGY_MODE_II )
KRATOS_CREATE_VARIABLE( double, FRACTURE_ENERGY_MODE_III )
KRATOS_CREATE_VARIABLE( double, EQUIVALENT_PLASTIC_DISPLACEMENT_JUMP_MODE_I )
KRATOS_CREATE_VARIABLE( double, EQUIVALENT_PLASTIC_DISPLACEMENT_JUMP_MODE_II )
KRATOS_CREATE_VARIABLE( double, EQUIVALENT_PLASTIC_DISPLACEMENT_JUMP_MODE_III )
KRATOS_CREATE_VARIABLE( double, INITIAL_COHESION )
KRATOS_CREATE_VARIABLE( double, INITIAL_FRICTION_ANGLE )
KRATOS_CREATE_VARIABLE( double, RESIDUAL_FRICTION_ANGLE )
KRATOS_CREATE_VARIABLE( double, INITIAL_DILATANCY_ANGLE )
KRATOS_CREATE_VARIABLE( double, RESIDUAL_DILATANCY_ANGLE )
KRATOS_CREATE_VARIABLE( double, INTERFACE_TENSILE_LAW_S0 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_S0 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_SP )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_SR )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_EP )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_C1 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_C2 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_C3 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_COMPRESSIVE_LAW_C4 )
KRATOS_CREATE_VARIABLE( double, INTERFACE_PLASTIC_DAMAGE_FACTOR_T )
KRATOS_CREATE_VARIABLE( double, INTERFACE_PLASTIC_DAMAGE_FACTOR_C )
KRATOS_CREATE_VARIABLE( double, INTERFACE_CAP_VALUE )
KRATOS_CREATE_VARIABLE( Vector, INTERFACE_TRACTION )
KRATOS_CREATE_VARIABLE( Vector, INTERFACE_DISPLACEMENT_JUMP )
KRATOS_CREATE_VARIABLE( Vector, INTERFACE_PLASTIC_DISPLACEMENT_JUMP )
KRATOS_CREATE_VARIABLE( double, YIELD_FUNCTION_VALUE )
KRATOS_CREATE_VARIABLE( int, INTERFACE_REDUCED_INTEGRATION )

// for plots
KRATOS_CREATE_VARIABLE( Vector, YIELD_SURFACE_DATA_2D_X )
KRATOS_CREATE_VARIABLE( Vector, YIELD_SURFACE_DATA_2D_Y )
KRATOS_CREATE_VARIABLE( Vector, YIELD_SURFACE_DATA_3D_X )
KRATOS_CREATE_VARIABLE( Vector, YIELD_SURFACE_DATA_3D_Y )
KRATOS_CREATE_VARIABLE( Vector, YIELD_SURFACE_DATA_3D_Z )

// for plastic law
KRATOS_CREATE_VARIABLE( double, ISOTROPIC_HARDENING )
KRATOS_CREATE_VARIABLE( double, KINEMATIC_HARDENING )
KRATOS_CREATE_VARIABLE( double, YIELD_STRESS_INFINITY )
KRATOS_CREATE_VARIABLE( double, ISOTROPIC_HARDENING_EXPONENT )
KRATOS_CREATE_VARIABLE( double, EQUIVALENT_PLASTIC_STRAIN )
KRATOS_CREATE_VARIABLE( Matrix, PLASTIC_STRAIN_TENSOR )

// for stabilized reduced integration
KRATOS_CREATE_VARIABLE( double, RI_STABILIZATION )
KRATOS_CREATE_VARIABLE( double, RI_STABILIZATION_RESIDUAL )

// for enhanced strain elements
KRATOS_CREATE_VARIABLE( double, ENH_STRAIN_PARAM_1 )
KRATOS_CREATE_VARIABLE( double, ENH_STRAIN_PARAM_2 )
KRATOS_CREATE_VARIABLE( double, ENH_STRAIN_PARAM_3 )
KRATOS_CREATE_VARIABLE( double, ENH_STRAIN_PARAM_4 )
KRATOS_CREATE_VARIABLE( double, ENH_STRAIN_PARAM_5 )

// misc
KRATOS_CREATE_VARIABLE( double, RANDOM_IMPERFECTION_FACTOR )
KRATOS_CREATE_VARIABLE( Vector, DISCONTINUITY_DIRECTION )
KRATOS_CREATE_VARIABLE( double, LAMBDA_OUTPUT )

}
