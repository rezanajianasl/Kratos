// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:         BSD License
//                   license: structural_mechanics_application/license.txt
//
//  Main authors:    Reza Najian Asl
//
// System includes
#include <iostream>

// External includes

// Project includes
#include "custom_constitutive/elastic_isotropic_damage_3d.h"
#include "includes/checks.h"

#include "structural_mechanics_application_variables.h"

namespace Kratos
{
/******************************CONSTRUCTOR******************************************/
/***********************************************************************************/

ElasticIsotropicDamage3D::ElasticIsotropicDamage3D()
    : ConstitutiveLaw()
{
}

/******************************COPY CONSTRUCTOR*************************************/
/***********************************************************************************/

ElasticIsotropicDamage3D::ElasticIsotropicDamage3D(const ElasticIsotropicDamage3D& rOther)
    : ConstitutiveLaw(rOther)
{
}

/********************************CLONE**********************************************/
/***********************************************************************************/

ConstitutiveLaw::Pointer ElasticIsotropicDamage3D::Clone() const
{
    return Kratos::make_shared<ElasticIsotropicDamage3D>(*this);
}

/*******************************DESTRUCTOR******************************************/
/***********************************************************************************/

ElasticIsotropicDamage3D::~ElasticIsotropicDamage3D()
{
};

/***********************************************************************************/
/***********************************************************************************/

void  ElasticIsotropicDamage3D::CalculateMaterialResponsePK2(ConstitutiveLaw::Parameters& rValues)
{
    KRATOS_TRY;
    // b.- Get Values to compute the constitutive law:
    Flags & r_constitutive_law_options = rValues.GetOptions();

    Vector& r_strain_vector = rValues.GetStrainVector();

    //NOTE: SINCE THE ELEMENT IS IN SMALL STRAINS WE CAN USE ANY STRAIN MEASURE. HERE EMPLOYING THE CAUCHY_GREEN
    if( r_constitutive_law_options.IsNot( ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN )) {
        CalculateCauchyGreenStrain( rValues, r_strain_vector);
    }

    if( r_constitutive_law_options.Is( ConstitutiveLaw::COMPUTE_STRESS )) {
        Vector& r_stress_vector = rValues.GetStressVector();
        CalculateDamagedPK2Stress( r_strain_vector, r_stress_vector, rValues);
    }

    if( r_constitutive_law_options.Is( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR )) {
        Matrix& r_constitutive_matrix = rValues.GetConstitutiveMatrix();
        CalculateDamagedConstitutiveMatrix( r_constitutive_matrix, rValues);
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

// NOTE: Since we are in the hypothesis of small strains we can use the same function for everything

void ElasticIsotropicDamage3D::CalculateMaterialResponsePK1 (ConstitutiveLaw::Parameters& rValues)
{
    CalculateMaterialResponsePK2(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateMaterialResponseKirchhoff (ConstitutiveLaw::Parameters& rValues)
{
    CalculateMaterialResponsePK2(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateMaterialResponseCauchy (ConstitutiveLaw::Parameters& rValues)
{
    CalculateMaterialResponsePK2(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::InitializeMaterialResponsePK1(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    InitializeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::InitializeMaterialResponsePK2(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    InitializeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::InitializeMaterialResponseCauchy(ConstitutiveLaw::Parameters& rValues)
{
    // TODO: Add if necessary
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::InitializeMaterialResponseKirchhoff(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    InitializeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::FinalizeMaterialResponsePK1(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    FinalizeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::FinalizeMaterialResponsePK2(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    FinalizeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::FinalizeMaterialResponseCauchy(ConstitutiveLaw::Parameters& rValues)
{
    // TODO: Add if necessary
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::FinalizeMaterialResponseKirchhoff(ConstitutiveLaw::Parameters& rValues)
{
    // Small deformation so we can call the Cauchy method
    FinalizeMaterialResponseCauchy(rValues);
}

/***********************************************************************************/
/***********************************************************************************/

double& ElasticIsotropicDamage3D::CalculateValue(ConstitutiveLaw::Parameters& rParameterValues, const Variable<double>& rThisVariable, double& rValue)
{
    Vector& r_strain_vector = rParameterValues.GetStrainVector();
    Vector& r_stress_vector = rParameterValues.GetStressVector();

    if (rThisVariable == STRAIN_ENERGY || rThisVariable == CRACK_STRAIN_ENERGY) {
        this->CalculateCauchyGreenStrain(rParameterValues, r_strain_vector);
        this->CalculateDamagedPK2Stress( r_strain_vector, r_stress_vector, rParameterValues);

        rValue = 0.5 * inner_prod( r_strain_vector, r_stress_vector); // Strain energy = 0.5*E:C:E
    }
    else if (rThisVariable == ELASTIC_STRAIN_ENERGY){
        this->CalculateCauchyGreenStrain(rParameterValues, r_strain_vector);
        this->CalculateElasticPK2Stress( r_strain_vector, r_stress_vector, rParameterValues);
        rValue = 0.5 * inner_prod( r_strain_vector, r_stress_vector); // Strain energy = 0.5*E:C:E
    }

    return( rValue );
}

/***********************************************************************************/
/***********************************************************************************/

Vector& ElasticIsotropicDamage3D::CalculateValue(
    ConstitutiveLaw::Parameters& rParameterValues,
    const Variable<Vector>& rThisVariable,
    Vector& rValue
    )
{
    if (rThisVariable == STRAIN ||
        rThisVariable == GREEN_LAGRANGE_STRAIN_VECTOR ||
        rThisVariable == ALMANSI_STRAIN_VECTOR) {
        this->CalculateCauchyGreenStrain( rParameterValues, rValue);
    } else if (rThisVariable == STRESSES ||
        rThisVariable == CAUCHY_STRESS_VECTOR ||
        rThisVariable == KIRCHHOFF_STRESS_VECTOR ||
        rThisVariable == CRACK_STRESS || 
        rThisVariable == ELASTIC_STRESS) {
        // Get Values to compute the constitutive law:
        Flags& r_flags = rParameterValues.GetOptions();

        // Previous flags saved
        const bool flag_const_tensor = r_flags.Is( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR );
        const bool flag_stress = r_flags.Is( ConstitutiveLaw::COMPUTE_STRESS );

        r_flags.Set( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true );
        r_flags.Set( ConstitutiveLaw::COMPUTE_STRESS, true );

        // We compute the stress
        ElasticIsotropicDamage3D::CalculateMaterialResponseCauchy(rParameterValues);
        rValue = rParameterValues.GetStressVector();

        // Previous flags restored
        r_flags.Set( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, flag_const_tensor );
        r_flags.Set( ConstitutiveLaw::COMPUTE_STRESS, flag_stress );
    } else if (rThisVariable == CRACK_STRESS || rThisVariable == ELASTIC_STRESS) {
        std::cout<<" HI REZA YOU ARE ASKING ME CRACK_STRESS and ELASTIC_STRESS"<<std::endl;
    }

    return( rValue );
}

/***********************************************************************************/
/***********************************************************************************/

Matrix& ElasticIsotropicDamage3D::CalculateValue(
    ConstitutiveLaw::Parameters& rParameterValues,
    const Variable<Matrix>& rThisVariable,
    Matrix& rValue
    )
{
    if (rThisVariable == CONSTITUTIVE_MATRIX ||
        rThisVariable == CONSTITUTIVE_MATRIX_PK2 ||
        rThisVariable == CONSTITUTIVE_MATRIX_KIRCHHOFF) {
        this->CalculateDamagedConstitutiveMatrix(rValue, rParameterValues);
    }else if(rThisVariable == DAMAGE_CONSTITUTIVE_MATRIX){
        std::cout<<" HI REZA YOU ARE ASKING ME DAMAGE_CONSTITUTIVE_MATRIX"<<std::endl;
        CheckClearElasticMatrix(rValue);
        rValue = IdentityMatrix(rValue.size1(),rValue.size2());
    }else if(rThisVariable == UNDAMAGE_CONSTITUTIVE_MATRIX){
        std::cout<<" HI REZA YOU ARE ASKING ME UNDAMAGE_CONSTITUTIVE_MATRIX"<<std::endl;
    }else if(rThisVariable == P_MATRIX){
        std::cout<<" HI REZA YOU ARE ASKING ME P_MATRIX"<<std::endl;
    }else if(rThisVariable == Ch_MATRIX){
        std::cout<<" HI REZA YOU ARE ASKING ME Ch_MATRIX"<<std::endl;
    }

    return( rValue );
}
/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::SetValue(
        const Variable<double>& rThisVariable,
        const double& rValue,
        const ProcessInfo& rCurrentProcessInfo
    )
{
    if(rThisVariable == DAMAGE) mDamage = rValue;
}
//*************************CONSTITUTIVE LAW GENERAL FEATURES *************************
/***********************************************************************************/

void ElasticIsotropicDamage3D::GetLawFeatures(Features& rFeatures)
{
    //Set the type of law
    rFeatures.mOptions.Set( THREE_DIMENSIONAL_LAW );
    rFeatures.mOptions.Set( INFINITESIMAL_STRAINS );
    rFeatures.mOptions.Set( ISOTROPIC );

    //Set strain measure required by the consitutive law
    rFeatures.mStrainMeasures.push_back(StrainMeasure_Infinitesimal);
    rFeatures.mStrainMeasures.push_back(StrainMeasure_Deformation_Gradient);

    //Set the strain size
    rFeatures.mStrainSize = 6;

    //Set the spacedimension
    rFeatures.mSpaceDimension = 3;
}

/***********************************************************************************/
/***********************************************************************************/

int ElasticIsotropicDamage3D::Check(
    const Properties& rMaterialProperties,
    const GeometryType& rElementGeometry,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    KRATOS_CHECK_VARIABLE_KEY(YOUNG_MODULUS);
    KRATOS_ERROR_IF(rMaterialProperties[YOUNG_MODULUS] < 0.0) << "YOUNG_MODULUS is negative." << std::endl;

    KRATOS_CHECK_VARIABLE_KEY(POISSON_RATIO);
    const double tolerance = 1.0e-12;
    const double nu_upper_bound = 0.5;
    const double nu_lower_bound = -1.0;
    const double nu = rMaterialProperties[POISSON_RATIO];
    KRATOS_ERROR_IF((nu_upper_bound - nu) < tolerance) << "POISSON_RATIO is above the upper bound 0.5." << std::endl;
    KRATOS_ERROR_IF((nu - nu_lower_bound) < tolerance) << "POISSON_RATIO is below the lower bound -1.0." << std::endl;

    KRATOS_CHECK_VARIABLE_KEY(DENSITY);
    KRATOS_ERROR_IF(rMaterialProperties[DENSITY] < 0.0) << "DENSITY is negative." << std::endl;

    return 0;
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CheckClearElasticMatrix(Matrix& rConstitutiveMatrix)
{
    const SizeType size_system = this->GetStrainSize();
    if (rConstitutiveMatrix.size1() != size_system || rConstitutiveMatrix.size2() != size_system)
        rConstitutiveMatrix.resize(size_system, size_system, false);
    rConstitutiveMatrix.clear();
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateElasticMatrix(
    Matrix& rConstitutiveMatrix,
    ConstitutiveLaw::Parameters& rValues
    )
{
    const Properties& r_material_properties = rValues.GetMaterialProperties();
    const double E = r_material_properties[YOUNG_MODULUS];
    const double NU = r_material_properties[POISSON_RATIO];

    this->CheckClearElasticMatrix(rConstitutiveMatrix);

    const double c1 = E / (( 1.00 + NU ) * ( 1 - 2 * NU ) );
    const double c2 = c1 * ( 1 - NU );
    const double c3 = c1 * NU;
    const double c4 = c1 * 0.5 * ( 1 - 2 * NU );

    rConstitutiveMatrix( 0, 0 ) = c2;
    rConstitutiveMatrix( 0, 1 ) = c3;
    rConstitutiveMatrix( 0, 2 ) = c3;
    rConstitutiveMatrix( 1, 0 ) = c3;
    rConstitutiveMatrix( 1, 1 ) = c2;
    rConstitutiveMatrix( 1, 2 ) = c3;
    rConstitutiveMatrix( 2, 0 ) = c3;
    rConstitutiveMatrix( 2, 1 ) = c3;
    rConstitutiveMatrix( 2, 2 ) = c2;
    rConstitutiveMatrix( 3, 3 ) = c4;
    rConstitutiveMatrix( 4, 4 ) = c4;
    rConstitutiveMatrix( 5, 5 ) = c4;
}
/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateDamagedConstitutiveMatrix(
    Matrix& rConstitutiveMatrix,
    ConstitutiveLaw::Parameters& rValues
    )
{
    
    Matrix unDamagedElasticMatrix;
    CalculateElasticMatrix(unDamagedElasticMatrix,rValues);

    double fD = (1-mDamage) * (1-mDamage); 
    Vector StrainVector = rValues.GetStrainVector();
    Matrix StrainTensor = MathUtils<double>::StrainVectorToTensor(StrainVector);
    double traceStrainTensor = 0.0;
    if (StrainVector.size()==3)
        traceStrainTensor = StrainVector[0] + StrainVector[1];
    else
        traceStrainTensor = StrainVector[0] + StrainVector[1] + StrainVector[2];
    int SigntraceStrainTensor = 1;
    if (traceStrainTensor<0)
        SigntraceStrainTensor = -1;

    const Properties& r_material_properties = rValues.GetMaterialProperties();
    const double E = r_material_properties[YOUNG_MODULUS];
    const double NU = r_material_properties[POISSON_RATIO];
    double k0 = E/(3*(1-2*NU));
    Matrix P = k0 * SigntraceStrainTensor * IdentityMatrix(unDamagedElasticMatrix.size1(),unDamagedElasticMatrix.size2());
    // now correct the constitutive matrix
    this->CheckClearElasticMatrix(rConstitutiveMatrix);
    rConstitutiveMatrix = fD * unDamagedElasticMatrix + (1-fD) * P; 

}
/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateDamagedPK2Stress(
    const Vector& rStrainVector,
    Vector& rStressVector,
    ConstitutiveLaw::Parameters& rValues
    )
{
    const Properties& r_material_properties = rValues.GetMaterialProperties();
    const double E = r_material_properties[YOUNG_MODULUS];
    const double NU = r_material_properties[POISSON_RATIO];


    double fD = (1-mDamage) * (1-mDamage);
    double traceStrainTensor = 0.0;
    if (rStrainVector.size()==3)
        traceStrainTensor = rStrainVector[0] + rStrainVector[1];
    else
        traceStrainTensor = rStrainVector[0] + rStrainVector[1] + rStrainVector[2];
    int SigntraceStrainTensor = 1;
    if (traceStrainTensor<0)
        SigntraceStrainTensor = -1;
    double k0 = E/(3*(1-2*NU));



    const double c1 = fD * (E / ((1.00 + NU) * (1 - 2 * NU)));
    const double c2 = fD * (c1 * (1 - NU)) + (1-fD) * k0 * SigntraceStrainTensor;
    const double c3 = fD * (c1 * NU);
    const double c4 = fD * (c1 * 0.5 * (1 - 2 * NU)) + (1-fD) * k0 * SigntraceStrainTensor;

    rStressVector[0] = c2 * rStrainVector[0] + c3 * rStrainVector[1] + c3 * rStrainVector[2];
    rStressVector[1] = c3 * rStrainVector[0] + c2 * rStrainVector[1] + c3 * rStrainVector[2];
    rStressVector[2] = c3 * rStrainVector[0] + c3 * rStrainVector[1] + c2 * rStrainVector[2];
    rStressVector[3] = c4 * rStrainVector[3];
    rStressVector[4] = c4 * rStrainVector[4];
    rStressVector[5] = c4 * rStrainVector[5];
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateElasticPK2Stress(
    const Vector& rStrainVector,
    Vector& rStressVector,
    ConstitutiveLaw::Parameters& rValues
    )
{
    const Properties& r_material_properties = rValues.GetMaterialProperties();
    const double E = r_material_properties[YOUNG_MODULUS];
    const double NU = r_material_properties[POISSON_RATIO];

    double traceStrainTensor = 0.0;
    if (rStrainVector.size()==3)
        traceStrainTensor = rStrainVector[0] + rStrainVector[1];
    else
        traceStrainTensor = rStrainVector[0] + rStrainVector[1] + rStrainVector[2];
    int SigntraceStrainTensor = 1;
    if (traceStrainTensor<0)
        SigntraceStrainTensor = -1;
    double k0 = E/(3*(1-2*NU));



    const double c1 = (E / ((1.00 + NU) * (1 - 2 * NU)));
    const double c2 = (c1 * (1 - NU)) - k0 * SigntraceStrainTensor;
    const double c3 = (c1 * NU);
    const double c4 = (c1 * 0.5 * (1 - 2 * NU)) - k0 * SigntraceStrainTensor;

    rStressVector[0] = c2 * rStrainVector[0] + c3 * rStrainVector[1] + c3 * rStrainVector[2];
    rStressVector[1] = c3 * rStrainVector[0] + c2 * rStrainVector[1] + c3 * rStrainVector[2];
    rStressVector[2] = c3 * rStrainVector[0] + c3 * rStrainVector[1] + c2 * rStrainVector[2];
    rStressVector[3] = c4 * rStrainVector[3];
    rStressVector[4] = c4 * rStrainVector[4];
    rStressVector[5] = c4 * rStrainVector[5];
}

/***********************************************************************************/
/***********************************************************************************/

void ElasticIsotropicDamage3D::CalculateCauchyGreenStrain(
    ConstitutiveLaw::Parameters& rValues,
    Vector& rStrainVector
    )
{
    const SizeType space_dimension = this->WorkingSpaceDimension();

    //1.-Compute total deformation gradient
    const Matrix& F = rValues.GetDeformationGradientF();
    KRATOS_DEBUG_ERROR_IF(F.size1()!= space_dimension || F.size2() != space_dimension)
        << "expected size of F " << space_dimension << "x" << space_dimension << ", got " << F.size1() << "x" << F.size2() << std::endl;

    Matrix E_tensor = prod(trans(F),F);
    for(unsigned int i=0; i<space_dimension; ++i)
      E_tensor(i,i) -= 1.0;
    E_tensor *= 0.5;

    noalias(rStrainVector) = MathUtils<double>::StrainTensorToVector(E_tensor);
}

} // Namespace Kratos
