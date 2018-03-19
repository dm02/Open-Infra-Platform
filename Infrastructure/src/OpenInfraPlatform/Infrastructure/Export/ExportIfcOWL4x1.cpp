/*
    This file is part of BlueFramework, a simple 3D engine.
    Copyright (c) 2016 Technical University of Munich
    Chair of Computational Modeling and Simulation.
    
    BlueFramework is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.
    
    BlueFramework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "OpenInfraPlatform/Infrastructure/Export/ExportIfcOWL4x1.h"
#include "OpenInfraPlatform/Infrastructure/Export/ExportIfcAlignment1x1.h"
#include "OpenInfraPlatform/ExpressBinding/Meta/Schema.h"
#include "raptor2/raptor2.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <typeinfo>

#include <OpenInfraPlatform/IfcAlignment1x1/IfcAlignment1x1EntitiesMap.h>

#include <BlueFramework/Engine/ResourceManagment/download.h>

#include <QString>
#include <QStringList>

class OpenInfraPlatform::Infrastructure::ExportIfcOWL4x1::ExportIfcOWL4x1Impl {
public:
	ExportIfcOWL4x1Impl(buw::ReferenceCounted<buw::AlignmentModel> am, buw::ReferenceCounted<buw::DigitalElevationModel> dem, const std::string& filename) {
		
		auto uuid = boost::uuids::uuid();
		std::string temp = "./";
		temp.append(boost::uuids::to_string(uuid));
		temp.append(".ifc");

		auto ifcExporter = buw::makeReferenceCounted<buw::ExportIfcAlignment1x1>(ifcAlignmentExportDescription(),am, dem, temp);
		boost::filesystem::remove(temp);
		
		auto model = ifcExporter->getIfcAlignment1x1Model();

		//Write header to file
		outfile = fopen(filename.c_str(), "w");

		world_ = raptor_new_world();

		if(boost::ends_with(filename, "rdf"))
			serializer_ = raptor_new_serializer(world_, "rdfxml-abbrev");
		else
			serializer_ = raptor_new_serializer(world_, "turtle");

		raptor_serializer_start_to_file_handle(serializer_, nullptr, outfile);

		const unsigned char* okstra_prefix = (const unsigned char*)"ifc";
		raptor_uri* okstra_uri = raptor_new_uri(world_, (const unsigned char*)"http://www.buildingsmart-tech.org/ifcowl/IFC4x1#");
		raptor_serializer_set_namespace(serializer_, okstra_uri, okstra_prefix);

		const unsigned char* rdf_prefix = (const unsigned char*)"rdf";
		raptor_uri* rdf_uri = raptor_new_uri(world_, (const unsigned char*)"http://www.w3.org/1999/02/22-rdf-syntax-ns#");
		raptor_serializer_set_namespace(serializer_, rdf_uri, rdf_prefix);
		

		for(auto entry : model->getMapIfcObjects()) {
			auto entity = entry.second;
			//std::cout << visit_struct::field_count(*entity) << std::endl;

			if(entity->classname() == "IfcPerson") {
				auto person = std::static_pointer_cast<OpenInfraPlatform::IfcAlignment1x1::IfcPerson>(entity);

				visit_struct::for_each(*person,
					[](const char * name, auto & value) {
					std::cout << name << " is a " << typeid(value).name() << std::endl;
					QString value_name = typeid(value).name();
					QStringList tokens = value_name.split("class ");
					tokens.filter("<");
					tokens.filter(">");
					for(auto word : tokens)
						std::cout << word.toStdString() << std::endl;
					
					

					

					/*if(typeid(value) == typeid(std::shared_ptr<OpenInfraPlatform::IfcAlignment1x1::IfcIdentifier>)) {
						if(typeid(*(value)) == typeid(OpenInfraPlatform::IfcAlignment1x1::IfcIdentifier)) {
							std::shared_ptr<OpenInfraPlatform::IfcAlignment1x1::IfcIdentifier> ptr = (std::shared_ptr<OpenInfraPlatform::IfcAlignment1x1::IfcIdentifier>) value;
						}
					}*/
					
				});
			}
		}


		
		/*
		std::vector<std::string> collectedAxisUniqueBlankTermNames;

		for (int i = 0; i < am->getAlignmentCount(); i++) {
			buw::ReferenceCounted<buw::IAlignment3D> alignment = am->getAlignment(i);
			buw::ReferenceCounted<buw::Alignment2DBased3D> alignment2D = std::static_pointer_cast<buw::Alignment2DBased3D>(alignment);
			buw::ReferenceCounted<buw::HorizontalAlignment2D> horizontalAlignment = alignment2D->getHorizontalAlignment();
			buw::ReferenceCounted<buw::VerticalAlignment2D> verticalAlignment = alignment2D->getVerticalAlignment();

			buw::String axisName = am->getAlignments()[i]->getName();

			std::stringstream ifcAlignmentUniqueBlankTermName;
			ifcAlignmentUniqueBlankTermName << "axis_" << i;

			collectedAxisUniqueBlankTermNames.push_back(ifcAlignmentUniqueBlankTermName.str());

			{
				raptor_statement* triple = nullptr;
				triple = raptor_new_statement(world_);

				triple->subject = raptor_new_term_from_blank(world_, (const unsigned char*)ifcAlignmentUniqueBlankTermName.str().c_str());
				triple->predicate = raptor_new_term_from_uri_string(world_, (const unsigned char*)"http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
				triple->object = raptor_new_term_from_uri_string(world_, (unsigned char*)"http://www.buildingsmart-tech.org/ifcowl/IFC4x1#IfcAlignment");

				raptor_serializer_serialize_statement(serializer_, triple);
				raptor_free_statement(triple);
			}

			{
				raptor_statement* triple = nullptr;
				triple = raptor_new_statement(world_);

				triple->subject = raptor_new_term_from_blank(world_, (const unsigned char*)ifcAlignmentUniqueBlankTermName.str().c_str());
				triple->predicate = raptor_new_term_from_uri_string(world_, (const unsigned char*)"http://www.buildingsmart-tech.org/ifcowl/IFC4x1#name_IfcRoot");
				triple->object = raptor_new_term_from_literal(world_, (unsigned char*)axisName.toCString(), nullptr, nullptr);

				raptor_serializer_serialize_statement(serializer_, triple);
				raptor_free_statement(triple);
			}



		}*/
	}

	virtual ~ExportIfcOWL4x1Impl() {
		raptor_serializer_serialize_end(serializer_);
		raptor_free_serializer(serializer_);

		raptor_free_world(world_);

		fclose(outfile);
	}

private:
	FILE * outfile = nullptr;
	raptor_world* world_ = nullptr;
	raptor_serializer* serializer_ = nullptr;
};

OpenInfraPlatform::Infrastructure::ExportIfcOWL4x1::ExportIfcOWL4x1(buw::ReferenceCounted<buw::AlignmentModel> am, buw::ReferenceCounted<buw::DigitalElevationModel> dem, const std::string& filename) : 
	Export(am, dem, filename), impl_(new ExportIfcOWL4x1Impl(am, dem, filename))
{

}

OpenInfraPlatform::Infrastructure::ExportIfcOWL4x1::~ExportIfcOWL4x1()
{

}
