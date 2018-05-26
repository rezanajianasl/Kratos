//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Riccardo Rossi
//                   Pooyan Dadvand
//

// System includes


// External includes


// Project includes
#include "includes/define.h"
#include "containers/model.h"
#include <iostream>
#include <string>
#include <sstream>


namespace Kratos
{
    void Model::Reset()
    {
        mRootModelPartMap.clear();
        mListOfVariablesLists.clear(); //this has to be done AFTER clearing the RootModelParts
    }
    
    ModelPart& Model::CreateModelPart( const std::string ModelPartName, ModelPart::IndexType NewBufferSize ) 
    {
        KRATOS_TRY
        
        KRATOS_INFO("Model") << "within CreateModelPart address of Model is " <<  &(*this) << std::endl; //TODO: remove - debugging purposes
        
        auto search = mRootModelPartMap.find(ModelPartName);
        if( search == mRootModelPartMap.end())
        {
//             KRATOS_INFO("Model") << ModelPartName << std::endl; //TODO: remove only for debugging purposes
            auto pvar_list = Kratos::make_unique<VariablesList>();
            mRootModelPartMap[ModelPartName] = Kratos::make_unique<ModelPart>(ModelPartName, NewBufferSize, pvar_list.get());
            mListOfVariablesLists.insert(std::move(pvar_list));
            return *(mRootModelPartMap[ModelPartName].get());
        }
        else
        {
            KRATOS_ERROR << "trying to create a root modelpart with name " << ModelPartName << " however a ModelPart with the same name already exists";
        }

        KRATOS_CATCH("")
    }

    void Model::DeleteModelPart( const std::string ModelPartName  ) 
    {
        KRATOS_TRY
        
        mRootModelPartMap.erase(ModelPartName);
        //NOTE: the corresponding variable list should NOT be removed

        KRATOS_CATCH("")
    }
    
    void Model::AddModelPart( ModelPart::Pointer pModelPart) //TODO: DEPRECATED. to be removed. this is TEMPORARY
    {
        KRATOS_TRY
        
        KRATOS_WARNING("Model") << "Model::AddModelPart is deprecated and is currently doing nothing" << std::endl;

//         //TODO: flat map should disappear in the future!!
//         auto search = mflat_map.find(pModelPart->Name());
//         if( search == mflat_map.end())
//         {
//             mflat_map[pModelPart->Name()] = pModelPart.get();
// 
//             //walk the submodelparts
//             for(auto& part : pModelPart->SubModelParts())
//                 AddModelPartRawPointer(&part);
//         }
//         else
//         {
//             if(&(*search->second) != &*(pModelPart.get()))
//                 KRATOS_ERROR << "trying to add to the Model two DISTINCT model parts with the same name. This should be possible (and it will be in the future) if they belong to two different root model_parts, but it is currently disallowed";
// 
//         }
// 
//         //add the root model part to the list
//         ModelPart& root_model_part = pModelPart->GetRootModelPart();
//         mRootModelPartMap[root_model_part.Name()] = &root_model_part;

        KRATOS_CATCH("")
    }

    void Model::AddModelPartRawPointer( ModelPart* pModelPart) //TODO: DEPRECATED. to be removed. this is TEMPORARY
    {
        KRATOS_TRY

        KRATOS_WARNING("Model") << "Model::AddModelPartRawPointer is deprecated and is currently doing nothing" << std::endl;

//         //TODO: flat map should disappear in the future!!
//         auto search = mflat_map.find(pModelPart->Name());
//         if( search == mflat_map.end())
//         {ModelPartName
//             mflat_map[pModelPart->Name()] = pModelPart;
// 
//             //walk the submodelparts
//             for(auto& part : pModelPart->SubModelParts())
//                 AddModelPartRawPointer(&part);
//         }
//         else
//         {
//             if(&(*search->second) != &*(pModelPart))
//                 KRATOS_ERROR << "trying to add to the Model two DISTINCT model parts with the same name. This should be possible (and it will be in the future) if they belong to two different root model_parts, but it is currently disallowed";
// 
//         }
// 
//         //add the root model part to the list
//         ModelPart& root_model_part = pModelPart->GetRootModelPart();
//         mRootModelPartMap[root_model_part.Name()] = &root_model_part;

        KRATOS_CATCH("")
    }

    ModelPart& Model::GetModelPart(const std::string& rFullModelPartName)
    {
        KRATOS_TRY
        
//         KRATOS_INFO("Model") << "within GetModelPart address of Model is " <<  &(*this) << std::endl; //TODO: remove, this is for debugging purposes

        KRATOS_ERROR_IF( rFullModelPartName.empty() ) << "Attempting to find a "
            << "ModelPart with empty name (\"\")!" << std::endl;
            
        std::vector< std::string > subparts_list;
        GetSubPartsList(rFullModelPartName, subparts_list);
        
        
        if(subparts_list.size() == 1) //it is a root model part
        {
            auto search = mRootModelPartMap.find(subparts_list[0]);
            if(search != mRootModelPartMap.end())
            {
                return *(search->second);
            }
            else //let's also search it as a flat name - a feature that SHOULD BE DEPRECATED
            {
                for(auto it = mRootModelPartMap.begin(); it!=mRootModelPartMap.end(); it++)
                {
                     ModelPart* pmodel_part = RecursiveSearchByName(subparts_list[0], (it->second.get()));
                     if(pmodel_part != nullptr) //give back the first one that was found
                         return *pmodel_part;
                }
                
                //if we are here we did not find it
//                 KRATOS_ERROR << "model part with name " << subparts_list[0] << " is not found either as root or as submodelpart of any level" << std::endl;
                KRATOS_ERROR << "The ModelPart named : \"" << subparts_list[0]
                     << "\" was not found as root-ModelPart. The total input string was \""
                     << rFullModelPartName << "\"" << std::endl;
            }
        }
        else //it is a submodelpart with the full name provided
        {
            KRATOS_INFO("Model") << rFullModelPartName << std::endl; //TODO: remove - only for debugging purposes
            auto search = mRootModelPartMap.find(subparts_list[0]);
            if(search != mRootModelPartMap.end())
            {
                ModelPart* p_model_part = (search->second).get();
                for(unsigned int i=1; i<subparts_list.size(); ++i)
                {
                    KRATOS_ERROR_IF_NOT(p_model_part->HasSubModelPart(subparts_list[i]))
                        << "The ModelPart named : \"" << subparts_list[i]
                        << "\" was not found as SubModelPart of : \""
                        << subparts_list[i-1] << "\". The total input string was \""
                        << rFullModelPartName << "\"" << std::endl;
                    p_model_part = &p_model_part->GetSubModelPart(subparts_list[i]);
                }
                return *p_model_part;
            }
            else 
            {
                KRATOS_ERROR << "root model part " << rFullModelPartName << " not found" << std::endl;
            }
            
        }

//         auto search = mflat_map.find(rFullModelPartName);
//         if(search != mflat_map.end()) {
//             // TODO enable the warning
//             // KRATOS_WARNING_IF("Model", (search->second)->IsSubModelPart())
//             //     << "Getting a SubModelPart from the Model without "
//             //     << "specifying the RootModelPart is deprecated and will be removed\n"
//             //     << "Please use e.g \"RootModelPart.SubModelPart.SubSubModelPart\" "
//             //     << "as input for this function" << std::endl;
//             return *(search->second);
//         }
//         else //look for it in the "root_map" which is where it is suppossed to be finally
//         {
//             std::vector< std::string > subparts_list;
//             GetSubPartsList(rFullModelPartName, subparts_list);
// 
//             //token 0 is the root
//             auto search = mRootModelPartMap.find(subparts_list[0]);
//             if(search != mRootModelPartMap.end())
//             {
//                 ModelPart* mp = search->second;
//                 for(unsigned int i=1; i<subparts_list.size(); i++)
//                 {
//                     KRATOS_ERROR_IF_NOT(mp->HasSubModelPart(subparts_list[i]))
//                         << "The ModelPart named : \"" << subparts_list[i]
//                         << "\" was not found as SubModelPart of : \""
//                         << subparts_list[i-1] << "\". The total input string was \""
//                         << rFullModelPartName << "\"" << std::endl;
//                     mp = &(mp->GetSubModelPart(subparts_list[i]));
//                 }
//                 return *mp;
//             }
//             else
//             {
//                 KRATOS_ERROR << "The ModelPart named : \"" << subparts_list[0]
//                     << "\" was not found as root-ModelPart. The total input string was \""
//                     << rFullModelPartName << "\"" << std::endl;
//             }
// 
//         }

        KRATOS_CATCH("")
    }

    bool Model::HasModelPart(const std::string& rFullModelPartName)
    {
        KRATOS_TRY

        KRATOS_ERROR_IF( rFullModelPartName.empty() ) << "Attempting to find a "
            << "ModelPart with empty name (\"\")!" << std::endl;

        std::vector< std::string > subparts_list;
        GetSubPartsList(rFullModelPartName, subparts_list);

        //token 0 is the root
        auto search = mRootModelPartMap.find(subparts_list[0]);
        if(search != mRootModelPartMap.end())
        {
            ModelPart* mp = (search->second).get();
            for(unsigned int i=1; i<subparts_list.size(); i++)
            {
                if(!mp->HasSubModelPart(subparts_list[i]))
                    return false;
                mp = &(mp->GetSubModelPart(subparts_list[i]));
            }
            return true;
        }
        else
        {
            return false;
        }

        KRATOS_CATCH("")
    }

    std::string Model::Info() const
    {
        std::stringstream ss;
        for(auto it = mRootModelPartMap.begin(); it!=mRootModelPartMap.end(); it++)
        {
             ss<< *((it->second).get()) << std::endl << std::endl;
        }
        return ss.str();
    }

    /// Print information about this object.

    void Model::PrintInfo(std::ostream& rOStream) const
    {
        rOStream << Info();
    }

    /// Print object's data.

    void Model::PrintData(std::ostream& rOStream) const
    {
    }

    void Model::GetSubPartsList(const std::string& rFullModelPartName,
                                std::vector<std::string>& rSubPartsList)
    {
        std::istringstream iss(rFullModelPartName);
        std::string token;
        rSubPartsList.clear();
        while (std::getline(iss, token, '.'))
        {
            rSubPartsList.push_back(token);
        }
    }
    
    ModelPart* Model::RecursiveSearchByName(const std::string& ModelPartName, ModelPart* pModelPart)
    {

        for(auto& part : pModelPart->SubModelParts())
        {
            if(part.Name() == ModelPartName)
                return &part;
            else
                RecursiveSearchByName(ModelPartName, &part);
        }
        return nullptr;
    }

}  // namespace Kratos.


