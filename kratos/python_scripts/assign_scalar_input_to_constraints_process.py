from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7
# Importing the Kratos Library
import KratosMultiphysics

def Factory(settings, Model):
    if not isinstance(settings, KratosMultiphysics.Parameters):
        raise Exception("expected input shall be a Parameters object, encapsulating a json string")
    return AssignScalarInputToConstraintsProcess(Model, settings["Parameters"])

from KratosMultiphysics import assign_scalar_input_to_entities_process

## All the processes python should be derived from "Process"
class AssignScalarInputToConstraintsProcess(assign_scalar_input_to_entities_process.AssignScalarInputToEntitiesProcess):
    """This process sets a variable a certain input value in a given direction, for all the constraints belonging to a submodelpart. Uses assign_scalar_input_to_constraints_process for each component

    Only the member variables listed below should be accessed directly.

    Public member variables:
    Model -- the container of the different model parts.
    settings -- Kratos parameters containing solver settings.
    """

    def __init__(self, Model, settings ):
        """ The default constructor of the class

        Keyword arguments:
        self -- It signifies an instance of a class.
        Model -- the container of the different model parts.
        settings -- Kratos parameters containing solver settings.
        """

        # The value can be a double or a string (function)
        default_settings = KratosMultiphysics.Parameters("""
        {
            "help"               : "This process assigns a given value (input) to all the constraints belonging a certain submodelpart",
            "mesh_id"            : 0,
            "model_part_name"    : "please_specify_model_part_name",
            "variable_name"      : "SPECIFY_VARIABLE_NAME",
            "interval"           : [0.0, 1e30],
            "file"               : "",
            "transfer_algorithm" : "nearest_neighbour",
            "entities"           : ["constraints"]
        }
        """
        )

        settings.ValidateAndAssignDefaults(default_settings)

        # Ensure proper entities
        if settings["entities"].size() != 1:
            settings["entities"] = default_settings["entities"]
        else:
            if settings["entities"][0].GetString() != "constraints":
                settings["entities"] = default_settings["entities"]

        # Construct the base process.
        super(AssignScalarInputToConstraintsProcess, self).__init__(Model, settings)