#include "storage/export_method.h"
#include "download/api_factory.h"
#include "download/api_loader.h"
#include "download/api_parameter.h"

unordered_map<ExportMethod::methods, shared_ptr<ExportMethod>>
    ExportMethod::available_methods = {
        {ExportMethod::methods::ZIP, shared_ptr<ZipExport>(new ZipExport())}};

string ExportMethod::compressed_export(std::list<shared_ptr<File>> files,
                                       string archive_name,
                                       ExportMethod::methods method) {
  return ExportMethod::available_methods.at(method)->compressed_export(
      files, archive_name);
}

void ExportMethod::init_api_tags(ExtractionApiTags &api_tags,
                                 std::list<shared_ptr<File>> &files) {
  set<string> api_names;
  for (auto &file : files) {
    api_names.insert(file->get_source());
  }

  std::shared_ptr<api_loader> loader;
  vector<shared_ptr<api_parameter_base>> curr_api_tags;
  string curr_api_tag_name;
  for (auto &api_name : api_names) {
    loader = ApiFactory::get_api(api_name);
    curr_api_tags = loader->get_relevant_parameters();
    for (auto &curr_api_tag : curr_api_tags) {
      curr_api_tag_name = curr_api_tag->get_name();
      api_tags[api_name].push_back(Tag(curr_api_tag_name, ""));
    }
  }
}

string ExportMethod::get_api_tags_titles(ExtractionApiTags api_tags) {
  string titles;
  ExtractionApiTags::iterator it = api_tags.begin();
  string api_name;
  std::list<Tag> tags;
  while (it != api_tags.end()) {
    api_name = it->first;
    titles += "\"" + api_name + "\"" + ",";
    tags = it->second;
    for (auto &tag : tags) {
      titles += "\"" + tag.get_name() + "\"" + ",";
    }
    it++;
  }
  titles.pop_back();
  return titles;
}

string ExportMethod::get_api_tags_values(ExtractionApiTags api_tags,
                                         string source) {
  string values;
  ExtractionApiTags::iterator it = api_tags.begin();
  std::list<Tag> tags;
  string api_name;
  while (it != api_tags.end()) {
    api_name = it->first;
    tags = it->second;
    if (api_name == source) {
      values += "1,";
    } else {
      values += "0,";
    }
    api_parameter_base::value_type parameter_type;
    for (auto &tag : tags) {
      parameter_type =
          ApiFactory::get_api(api_name)->get_relevant_parameter_type(
              tag.get_name());
      if ((parameter_type == api_parameter_base::value_type::STRING ||
           parameter_type == api_parameter_base::value_type::IMAGE_LINK) &&
          tag.get_value() != "") {
        values += "\"" + tag.get_value() + "\"" + ",";
      } else {
        values += tag.get_value() + ",";
      }
    }
    it++;
  }
  values.pop_back();
  return values;
}

void ExportMethod::clear_api_tags(ExtractionApiTags &api_tags) {
  ExtractionApiTags::iterator it = api_tags.begin();
  list<Tag> tags;
  while (it != api_tags.end()) {
    tags = it->second;
    for (auto &tag : tags) {
      tag.set_value("");
    }
    api_tags[it->first] = tags;
    it++;
  }
}

string ExportMethod::get_metadata_content(std::list<shared_ptr<File>> files) {
  stringstream content;
  ExtractionApiTags api_tags;
  init_api_tags(api_tags, files);
  content << File::get_metadata_titles() << "," << get_api_tags_titles(api_tags)
          << endl;
  for (auto &file : files) {
    file->fill_extraction_tags(api_tags);
    content << file->get_metadata_values() << ","
            << get_api_tags_values(api_tags, file->get_source()) << endl;
    clear_api_tags(api_tags);
  }
  return content.str();
}

void ExportMethod::add_metadata(std::list<shared_ptr<File>> files,
                                std::string tmp_path) {
  string metadata_path = tmp_path + METADATA_FILE_NAME;
  ofstream metadata(metadata_path);
  if (!metadata.is_open()) {
    return;
  }
  metadata << get_metadata_content(files);
  metadata.close();
  add_file(metadata_path, METADATA_FILE_NAME);
}
