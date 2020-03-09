// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:             BSD License
//                                       license: StructuralMechanicsApplication/license.txt
//
//  Main authors:    Manuel Messmer
//                   
//

// System includes

// External includes
#include <Eigen/Core>
#include <Eigen/Dense>

// Project includes
#include "custom_processes/pertube_geometry.h"
#include "utilities/builtin_timer.h"

#include "utilities/openmp_utils.h"



namespace Kratos
{ 

typedef ModelPart::NodesContainerType::ContainerType                 ResultNodesContainerType;

int PertubeGeometryProcess::CreateEigenvectors( ModelPart& rThisModelPart, double minDistance, double correlationLength, double truncationTolerance ){
    KRATOS_TRY;
    
    int NumOfNodes = rThisModelPart.NumberOfNodes();
    KRATOS_WATCH( NumOfNodes );
    // double minDistance = 0.05;
    // double correlationLength = 0.5;

    BuiltinTimer reduceModel;

    searcher = new OMP_NodeSearch;
    ModelPart::NodesContainerType nodes = rThisModelPart.Nodes();
    ModelPart::NodesContainerType nodes2 = {};
    double radius = minDistance;
    ResultNodesContainerType  results;
    std::vector<std::vector<double>> resutlsDistance;
    
    searcher->InitializeSearch(nodes);
    std::vector<ModelPart::NodeIterator> reduced_space_nodes;
    int counter = 1;
    ModelPart::NodeIterator it_n = rThisModelPart.NodesBegin();
    
    for (ModelPart::NodeIterator itNode = rThisModelPart.NodesBegin(); itNode != rThisModelPart.NodesEnd(); itNode++)
    {   
        itNode->Set(VISITED,false);
    }
    for (ModelPart::NodeIterator itNode =rThisModelPart.NodesBegin(); itNode != rThisModelPart.NodesEnd(); itNode++)
    {   
        if( !itNode->Is(VISITED) ) {
            itNode->Set(VISITED,true);
            reduced_space_nodes.push_back(itNode);
            results = {};
            searcher->SearchNodesInRadiusExclusiveImplementation(nodes,itNode->GetId()-1,radius,results);
            for( int i = 0; i < results.size(); i++ ){
                results[i]->Set(VISITED,true);
            }
        }
    }
    KRATOS_INFO("System Construction Time")
            << reduceModel.ElapsedSeconds() << std::endl;
    
    Eigen::MatrixXd CorrelationMatrix; 
    double redSpace = reduced_space_nodes.size();
    KRATOS_WATCH( redSpace );
    std::cout << "Size: " <<  reduced_space_nodes.size() << std::endl;
    CorrelationMatrix.resize(redSpace,redSpace);
    int row_counter = -1;
    for ( std::vector<ModelPart::NodeIterator>::iterator it = reduced_space_nodes.begin(); it != reduced_space_nodes.end(); it++ )
    {
        row_counter++;
        int column_counter = -1;
        for( std::vector<ModelPart::NodeIterator>::iterator it_inner = reduced_space_nodes.begin(); it_inner != reduced_space_nodes.end(); it_inner++ )
        {
            column_counter++;
            CorrelationMatrix(row_counter ,column_counter ) =  CorrelationFunction( (*it), (*it_inner), correlationLength);  
        }
    }
    
    // Solve Eigenvalue Problem
    Parameters params(R"(
        {
            "solver_type": "eigen_eigensystem",
            "number_of_eigenvalues": 5,
            "normalize_eigenvectors": true,
            "max_iteration": 100,
            "tolerance": 1e-6,
            "echo_level": 1
        })");
    //KRATOS_WATCH(CorrelationMatrix);
    std::cout << "Eigensolver start: " << std::endl;
    BuiltinTimer Eigensolver_time;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(redSpace);
    es.compute(CorrelationMatrix,Eigen::ComputeEigenvectors);
    KRATOS_INFO("Eigensolver Time")
            << Eigensolver_time.ElapsedSeconds() << std::endl;
    std::cout << "Eigensolver done: " << std::endl;
    //es.compute(CorrelationMatrix,Eigen::ComputeEigenvectors);
    if( es.info() != Eigen::Success)
    {
        std::cout << "Decomposition failed!" << std::endl;  
    }

    // Find number of neccessary eigenvectors
    // KRATOS_WATCH( es.eigenvectors().col(0) );


    double sum_eig =  es.eigenvalues()( es.eigenvalues().size()-1 );
    // int numb = 0;
    // double espilon = 0.0;
    // for( int i = es.eigenvalues().size() - 2; i >= 0; i--)
    // {
    //     espilon = 1 - sum_eig / ( sum_eig + es.eigenvalues()(i) );
    //     numb++;
    //     //KRATOS_WATCH( espilon );
    //     if( espilon < truncationTolerance)
    //     {
    //         std::cout << "Truncation converged with a tolerance of:  " <<  truncationTolerance << std::endl;
    //         std::cout << "Needed Eigenvectors: " << numb << std::endl;
    //         break;
    //     }
    //     else if( i == es.eigenvalues().size() - 1)
    //     {
    //         numb = es.eigenvalues().size();
    //         std::cout << "Truncation NOT concerged, achieved tolerance:  " <<  espilon << " / " << 0.01 << std::endl;
    //         std::cout << "Maximum number of computed eigenvalues is used: " << numb << std::endl;
    //     }
    //     sum_eig += es.eigenvalues()(i);
    // }
    
    // Different criteria
    // ###################################################################################
    double sum_a = 0.0;
    double sum_b = 0.0;
    //KRATOS_WATCH( es.eigenvalues() );
    int numb = 0;
    double espilon = 0.0;
    for( int i = es.eigenvalues().size() - 1; i >= 0; i--)
    {
        sum_a += es.eigenvalues()(i);
        //std::cout << "a: " << sum_a << std::endl;
    }
    for( int i = es.eigenvalues().size() - 1; i >= 0; i--)
    {
        sum_b += es.eigenvalues()(i);
        
        numb++;
        if( sum_b > truncationTolerance*sum_a)
        {
            std::cout << "Truncation converged with a tolerance of:  " <<  truncationTolerance << std::endl;
            std::cout << "Needed Eigenvectors: " << numb << std::endl;
            break;
        }
    }
    std::cout << "espilone2: " << 1 - sum_a/sum_b << std::endl;
    // ######################################################################################
    
    int NumOfRandomVariables = numb;
    // Truncate Matrix
    Eigen::MatrixXd Eigenvectors2 = es.eigenvectors();

    int cutOff = redSpace - NumOfRandomVariables;
    Eigen::MatrixXd Eigenvectors = Eigen::Map<Eigen::MatrixXd,0,Eigen::OuterStride<> >(
    Eigenvectors2.real().data() + cutOff , es.eigenvectors().real().rows(), NumOfRandomVariables, Eigen::OuterStride<>(es.eigenvectors().outerStride()) ) ;
    //KRATOS_WATCH( Eigenvectors );
    // Truncate Vector
    //KRATOS_WATCH( es.eigenvalues() );
    Eigen::VectorXd Eigenvalues = es.eigenvalues().real().tail(NumOfRandomVariables);
    //KRATOS_WATCH( Eigenvalues );

    int j = -1;

    Displacement = Eigen::MatrixXd::Zero(NumOfNodes,NumOfRandomVariables); 
    Eigen::VectorXd CorrelationVector = Eigen::VectorXd::Zero(redSpace);

    for (ModelPart::NodeIterator itNode = rThisModelPart.NodesBegin(); itNode != rThisModelPart.NodesEnd(); itNode++)
    {
        j++;
        double tmp = 0;
        CorrelationVector = Eigen::RowVectorXd::Zero(redSpace);
        int k = -1;
        for( std::vector<ModelPart::NodeIterator>::iterator it = reduced_space_nodes.begin(); it != reduced_space_nodes.end(); it++){
            k++;
            // Remove if statement
            if( (*it) == itNode )
            {
                CorrelationVector(k) = CorrelationFunction( itNode, (*it), correlationLength);
            }
            else {
                CorrelationVector(k) = CorrelationFunction( itNode, (*it), correlationLength);
            }
        }

        for( int i = 0; i < NumOfRandomVariables; i++)
        {
            Displacement(j,i) = sqrt( 1/Eigenvalues(i) ) * (CorrelationVector).dot(Eigenvectors.col(i));

        }
        
    }
    
    return NumOfRandomVariables;
    KRATOS_CATCH("");
}

void PertubeGeometryProcess::AssembleEigenvectors( ModelPart& rThisModelPart, const std::vector<double>& variables )
{
    //Todo: Check if NumOfRandomVariables==NumberOfEigenvectors
    const int NumOfRandomVariables = variables.size();
    const int NumOfNodes = rThisModelPart.NumberOfNodes();
    KRATOS_WATCH( NumOfRandomVariables );
    //KRATOS_WATCH( variables );
    int j = -1;
    array_1d<double, 3> normal;
    std::vector<double> randomField(NumOfNodes,0.0);
    

    for (ModelPart::NodeIterator itNode = rThisModelPart.NodesBegin(); itNode != rThisModelPart.NodesEnd(); itNode++)
    {
        j++;
        for( int i = 0; i < NumOfRandomVariables; i++)
        {
            randomField[j] += variables[i]*Displacement(j,i);
        }
    }
    
    double shifted_mean = 1.0 / NumOfNodes * std::accumulate(randomField.begin(), randomField.end(), 0.0);
    // Scale and shift random Field
    for(auto& element : randomField){
        element = element - shifted_mean;
    }
    //std::cout << "Shifted Mean: " << shifted_mean << std::endl;
    double max_disp = *std::max_element(randomField.begin(), randomField.end());
    double min_disp = *std::min_element(randomField.begin(), randomField.end());

    double max_abs_disp = std::max( std::abs(max_disp), std::abs(min_disp) );
    //std::cout << "Maximal Displacement: " << max_abs_disp << std::endl;
    for(auto& element : randomField){
        element = mMaximalDisplacement/max_abs_disp*element;
    } 

    max_disp = *std::max_element(randomField.begin(), randomField.end());
    min_disp = *std::min_element(randomField.begin(), randomField.end());

    max_abs_disp = std::max( std::abs(max_disp), std::abs(min_disp) );
    //std::cout << "Maximal Displacement: " << max_abs_disp << std::endl;
    j = -1;
    ModelPart::NodeIterator itNode_intial = mrInitialModelPart.NodesBegin();
    int x = 0;
    int y = 0;
    for (ModelPart::NodeIterator itNode = rThisModelPart.NodesBegin(); itNode != rThisModelPart.NodesEnd(); itNode++)
    {
        j++;
        double tmp = 0.0;
        normal =  itNode_intial->FastGetSolutionStepValue(NORMAL);

        itNode_intial = itNode_intial + 1;
        //itNode->GetInitialPosition().Coordinates() += normal*randomField[j];
        //Apply on plane ##########################
        
        if(x % 300 == 0){ x = 0; y++;}
        itNode->GetInitialPosition().Coordinates()(0) = x;
        itNode->GetInitialPosition().Coordinates()(1) = y;
        itNode->GetInitialPosition().Coordinates()(2) = 0.0; //randomField[j];
        x++;
        itNode->FastGetSolutionStepValue(DISPLACEMENT_Z) += randomField[j];
        //#########################################
        // KRATOS_WATCH( itNode->GetInitialPosition().Coordinates() );
        //itNode->GetInitialPosition().Coordinates()(1) += maxDisplacement*variables[i]*Displacement(j,i);
    }
    //std::cout << "Maximal Displacement: " << max_disp << std::endl;
}


double PertubeGeometryProcess::CorrelationFunction( ModelPart::NodeIterator itNode1, ModelPart::NodeIterator itNode2, double CorrelationLenth)
{
    // array_1d<double, 3> coorrdinate;
    // coorrdinate = itNode1->GetInitialPosition().Coordinates() - itNode2->GetInitialPosition().Coordinates();

    // double norm = sqrt( coorrdinate(0)*coorrdinate(0) + coorrdinate(1)*coorrdinate(1) + coorrdinate(2)*coorrdinate(2) );

    //This is for cylinder coordinates
    double Radius = 250.0;
    array_1d<double, 3> coordinate1;
    array_1d<double, 3> coordinate2;
    coordinate1 = itNode1->GetInitialPosition().Coordinates();
    coordinate2 = itNode2->GetInitialPosition().Coordinates();

    double pi = 3.14159265358979323846264338327950288419716939937510;

    double phi1 = atan2(coordinate1(1), coordinate1(2));
    double phi2 = atan2(coordinate2(1), coordinate2(2));
    // std::cout << "--: "<< atan2(-0.001,-1) << std::endl;
    // std::cout << "++: " <<  atan2(0.001,-1) << std::endl;
    // if( phi1 < 0.0 ){
    //     phi1 += 2*pi;
    // }
    // if( phi2 < 0.0 ){
    //     phi2 += 2*pi;
    // }

    double phi = std::abs(phi1 - phi2);
    if( phi > pi )
    {
        phi = 2.0*pi - phi;
    }
    double delta_circumference = std::abs(phi) * Radius;

    double delta_x = (coordinate1(0) - coordinate2(0));

    double norm = sqrt( delta_circumference*delta_circumference + delta_x*delta_x );
    //std::cout << "Norm for test values: " << norm << std::endl;
    
    return( exp( - norm / (CorrelationLenth) ) );  
}



} // namespace Kratos