from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing Kratos
import KratosMultiphysics
import KratosMultiphysics.StructuralMechanicsApplication as StructuralMechanicsApplication

# Importing the solvers (if available)
try:
    import KratosMultiphysics.ExternalSolversApplication
    KratosMultiphysics.Logger.PrintInfo("ExternalSolversApplication", "succesfully imported")
except ImportError:
    KratosMultiphysics.Logger.PrintInfo("ExternalSolversApplication", "not imported")
try:
    import KratosMultiphysics.EigenSolversApplication
    KratosMultiphysics.Logger.PrintInfo("EigenSolversApplication", "succesfully imported")
except ImportError:
    KratosMultiphysics.Logger.PrintInfo("EigenSolversApplication", "not imported")


class StructuralMechanicsAnalysis(object): # TODO in the future this could derive from a BaseClass in the Core
    """
    This class is the main-script of the StructuralMechanicsApplication put in a class

    It can be imported and used as "black-box"
    """
    def __init__(self, ProjectParameters):
        if (type(ProjectParameters) != KratosMultiphysics.Parameters):
            raise Exception("Input is expected to be provided as a Kratos Parameters object")
        self.ProjectParameters = ProjectParameters

    #### Public functions to run the Analysis ####
    def Run(self):
        self.Initialize()
        self.RunMainTemporalLoop()
        self.Finalize()

    def RunMainTemporalLoop(self):
        while self.time < self.end_time:
            self.InitializeTimeStep()
            self.SolveTimeStep()
            self.FinalizeTimeStep()

    #### Public functions defining the Interface to the CoSimulationApplication ####
    def Initialize(self, external_model_part=None):
        self.ImportAndCreateSolver(external_model_part)
        self.InitializeIO()
        self.ExecuteInitialize()
        self.ExecuteBeforeSolutionLoop()

    def InitializeTimeStep(self):
        self.ExecuteInitializeSolutionStep()

    def SolveTimeStep(self):
        self.SolveSolutionStep();

    def FinalizeTimeStep(self):
        self.ExecuteFinalizeSolutionStep()

    def Finalize(self):
        self.ExecuteFinalize()


    ###########################################################################
    def ImportAndCreateSolver(self, external_model_part=None):
        """ Importing the Solver and the ModelPart """
        if external_model_part != None:
            # This is a temporary solution until the importing of the ModelPart
            # is removed from the solver (needed e.g. for Optimization)
            if (type(external_model_part) != KratosMultiphysics.ModelPart):
                raise Exception("Input is expected to be provided as a Kratos ModelPart object")
            using_external_model_part = True
        else:
            using_external_model_part = False

        ## Get echo level and parallel type
        self.echo_level = self.ProjectParameters["problem_data"]["echo_level"].GetInt()
        self.parallel_type = self.ProjectParameters["problem_data"]["parallel_type"].GetString()

        # To avoid many prints # TODO leave this?
        if (self.echo_level == 0):
            KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)

        ## Import parallel modules if needed
        if (self.parallel_type == "MPI"):
            import KratosMultiphysics.mpi as KratosMPI
            import KratosMultiphysics.MetisApplication as MetisApplication
            import KratosMultiphysics.TrilinosApplication as TrilinosApplication

        ## Structure model part definition
        if using_external_model_part:
            self.main_model_part = external_model_part
        else:
            main_model_part_name = self.ProjectParameters["problem_data"]["model_part_name"].GetString()
            self.main_model_part = KratosMultiphysics.ModelPart(main_model_part_name)
            self.main_model_part.ProcessInfo.SetValue(KratosMultiphysics.DOMAIN_SIZE,
                                                      self.ProjectParameters["problem_data"]["domain_size"].GetInt())

        ## Solver construction
        import python_solvers_wrapper_structural
        self.solver = python_solvers_wrapper_structural.CreateSolver(self.main_model_part, self.ProjectParameters)

        if not using_external_model_part:
            self.solver.AddVariables()

            ## Read the model - note that SetBufferSize is done here
            self.solver.ImportModelPart()

            ## Add AddDofs
            self.solver.AddDofs()

        ## Creation of the Kratos model (build sub_model_parts or submeshes)
        self.structure_model = KratosMultiphysics.Model()
        self.structure_model.AddModelPart(self.main_model_part)

    def InitializeIO(self):
        """ Initialize GiD  I/O """
        self.output_post  = self.ProjectParameters.Has("output_configuration")
        if (self.output_post == True):
            if (self.parallel_type == "OpenMP"):
                from gid_output_process import GiDOutputProcess as output_process
            elif (self.parallel_type == "MPI"):
                from gid_output_process_mpi import GiDOutputProcessMPI as output_process

            self.gid_output = output_process(self.solver.GetComputingModelPart(),
                                             self.ProjectParameters["problem_data"]["problem_name"].GetString(),
                                             self.ProjectParameters["output_configuration"])

            self.gid_output.ExecuteInitialize()

    def ExecuteInitialize(self):
        """ Initializing the Analysis"""
        ## Print model_part and properties
        if ((self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0)) and (self.echo_level > 1):
            KratosMultiphysics.Logger.PrintInfo("ModelPart", self.main_model_part)
            for properties in self.main_model_part.Properties:
                KratosMultiphysics.Logger.PrintInfo("Property " + str(properties.Id), properties)

        ## Processes construction
        import process_factory
        self.list_of_processes = process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["constraints_process_list"])
        self.list_of_processes += process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["loads_process_list"])
        if (self.ProjectParameters.Has("list_other_processes") == True):
            self.list_of_processes += process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["list_other_processes"])
        if (self.ProjectParameters.Has("json_output_process") == True):
            self.list_of_processes += process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["json_output_process"])
        # Processes for tests
        if (self.ProjectParameters.Has("json_check_process") == True):
            self.list_of_processes += process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["json_check_process"])
        if (self.ProjectParameters.Has("check_analytic_results_process") == True):
            self.list_of_processes += process_factory.KratosProcessFactory(self.structure_model).ConstructListOfProcesses(self.ProjectParameters["check_analytic_results_process"])

        if ((self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0)) and (self.echo_level > 1):
            count = 0
            for process in self.list_of_processes:
                count += 1
                # KratosMultiphysics.Logger.PrintInfo("Process " + str(count), process) # FIXME

        ## Processes initialization
        for process in self.list_of_processes:
            process.ExecuteInitialize()

        ## Solver initialization
        self.solver.Initialize()

    def ExecuteBeforeSolutionLoop(self):
        """ Perform Operations before the SolutionLoop """
        if (self.output_post == True):
            self.gid_output.ExecuteBeforeSolutionLoop()

        for process in self.list_of_processes:
            process.ExecuteBeforeSolutionLoop()

        ## Writing the full ProjectParameters file before solving
        if ((self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0)) and (self.echo_level > 1):
            f = open("ProjectParametersOutput.json", 'w')
            f.write(self.ProjectParameters.PrettyPrintJsonString())
            f.close()

        ## Stepping and time settings
        self.delta_time = self.ProjectParameters["problem_data"]["time_step"].GetDouble()
        start_time = self.ProjectParameters["problem_data"]["start_time"].GetDouble()
        self.end_time = self.ProjectParameters["problem_data"]["end_time"].GetDouble()

        if self.main_model_part.ProcessInfo[KratosMultiphysics.IS_RESTARTED] == True:
            self.time = self.main_model_part.ProcessInfo[KratosMultiphysics.TIME]
        else:
            self.time = start_time
            self.main_model_part.ProcessInfo[KratosMultiphysics.STEP] = 0

        if (self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0):
            KratosMultiphysics.Logger.PrintInfo("::[KSM Simulation]:: ", "Analysis -START- ")

    def ExecuteInitializeSolutionStep(self):
        """ Initialize the timestep and advance in time. Called once per timestep """
        self.time += self.delta_time
        self.main_model_part.ProcessInfo[KratosMultiphysics.STEP] += 1
        self.main_model_part.CloneTimeStep(self.time)

        if (self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0):
            KratosMultiphysics.Logger.PrintInfo("STEP: ", self.main_model_part.ProcessInfo[KratosMultiphysics.STEP])
            KratosMultiphysics.Logger.PrintInfo("TIME: ", self.time)

        for process in self.list_of_processes:
            process.ExecuteInitializeSolutionStep()

        if (self.output_post == True):
            self.gid_output.ExecuteInitializeSolutionStep()

    def ExecuteBeforeSolve(self):
        """ Function to be called before solving. Can be executed several times per timestep """
        pass

    def SolveSolutionStep(self):
        """ Solving one step. Can be called several times per timestep """
        self.ExecuteBeforeSolve()
        self.solver.Solve()
        self.ExecuteAfterSolve()

    def ExecuteAfterSolve(self):
        """ Function to be called after solving. Can be executed several times per timestep """
        pass

    def ExecuteFinalizeSolutionStep(self):
        """ Finalizing the timestep and printing the output. Called once per timestep """
        for process in self.list_of_processes:
            process.ExecuteFinalizeSolutionStep()

        if (self.output_post == True):
            self.gid_output.ExecuteFinalizeSolutionStep()

        for process in self.list_of_processes:
            process.ExecuteBeforeOutputStep()

        if (self.output_post == True) and (self.gid_output.IsOutputStep()):
            self.gid_output.PrintOutput()

        for process in self.list_of_processes:
            process.ExecuteAfterOutputStep()

        self.solver.SaveRestart() # whether a restart-file is written is decided internally

    def ExecuteFinalize(self):
        """ Operations to be performed at the end of the Analysis """
        for process in self.list_of_processes:
            process.ExecuteFinalize()

        if (self.output_post == True):
            self.gid_output.ExecuteFinalize()

        if (self.parallel_type == "OpenMP") or (KratosMPI.mpi.rank == 0):
            KratosMultiphysics.Logger.PrintInfo("::[KSM Simulation]:: ", "Analysis -END- ")


if __name__ == "__main__":
    from sys import argv

    if len(argv) > 2:
        err_msg =  'Too many input arguments!\n'
        err_msg += 'Use this script in the following way:\n'
        err_msg += '- With default ProjectParameters (read from "ProjectParameters.json"):\n'
        err_msg += '    "python3 class_structural_mechanics.py"\n'
        err_msg += '- With custom ProjectParameters:\n'
        err_msg += '    "python3 class_structural_mechanics.py CustomProjectParameters.json"\n'
        raise Exception(err_msg)

    if len(argv) == 2: # ProjectParameters is being passed from outside
        project_parameters_file_name = argv[1]
    else: # using default name
        project_parameters_file_name = "ProjectParameters.json"

    with open(project_parameters_file_name,'r') as parameter_file:
        ProjectParameters = KratosMultiphysics.Parameters(parameter_file.read())

    StructuralMechanicsAnalysis(ProjectParameters).Run()