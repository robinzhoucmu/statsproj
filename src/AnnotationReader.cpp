#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "AnnotationReader.h"

using namespace std;

AnnotationReader::AnnotationReader(const string& directory, const string& folder)
{
  char annotations_lst_dir[256];
  sprintf(annotations_lst_dir, "%s/%s/annotations.lst", directory.c_str(), folder.c_str());

  ifstream annotations_lst(annotations_lst_dir);
  string line;

  //read through list, get all filenames
  vector<string> annotations_filenames;
  while (annotations_lst.good())
  {
    getline(annotations_lst, line);
    annotations_filenames.push_back(line);
  }

  vector<string>::iterator it;
  for (it = annotations_filenames.begin(); it < annotations_filenames.end(); it++)
  {
    Image_Info new_img; 

    char annotation_filename[256];
    sprintf(annotation_filename, "%s/%s", directory.c_str(), (*it).c_str());

    ifstream annotation(annotation_filename);

    int looking_for = 0;
    int number_people_im = 0;
    int number_people_sofar = 0;
    while (annotation.good())
    {
      getline(annotation, line);

      //get rid of comments and whitespace
      int first_nonwhitespace = line.find_first_not_of(' ');
      if (first_nonwhitespace < 0 || line[first_nonwhitespace] == '#')
        continue;

      switch (looking_for)
      {
        case 0: //filename
        {
          int filename_ind = line.find("ilename");
          if (filename_ind < 0)
            continue;

          int quote_ind = line.find('"');
          int quote_ind2 = line.substr(quote_ind+1, -1).find('"');

          //look for filename
          new_img.image_name = directory;
          new_img.image_name += "/";
          new_img.image_name += line.substr(quote_ind+1, quote_ind2);

          looking_for = 1;

        }
        break;

        case 1: //number people
        {
          int g_truth_ind = line.find("ground truth");
          if (g_truth_ind < 0)
            continue;


          int number_ind = line.find_first_of("0123456789");

          number_people_im = atoi(line.substr(number_ind,-1).c_str());

          looking_for = 2;
        }
        break;

        case 2: //process each person, assuming there are 3 lines per person
        {
          Img_Coordinates img_coords;
          getline(annotation, line); //have to skip one line for label
          //this line has center point
          
          int next_index = line.find(':');

          getNextNumberInString(line, next_index, img_coords.x_cen, next_index);
          getNextNumberInString(line, next_index, img_coords.y_cen, next_index);


          getline(annotation, line); //get line that contains bounding box
          next_index = line.find(':');
          getNextNumberInString(line, next_index, img_coords.x_min, next_index);
          getNextNumberInString(line, next_index, img_coords.y_min, next_index);
          getNextNumberInString(line, next_index, img_coords.x_max, next_index);
          getNextNumberInString(line, next_index, img_coords.y_max, next_index);


          new_img.people_coordinates.push_back(img_coords);
        }

        default:
        {
          continue;
        }
      }
    }

    if (looking_for != 0)
      _images_info.push_back(new_img);

    //print for sanity check
//    cout << _images_info.size() << endl;
//    cout << _images_info.back().image_name << endl;
//    for (size_t i = 0; i < _images_info.back().people_coordinates.size(); i++)
//    {
//      Img_Coordinates img_coords = _images_info.back().people_coordinates[i];
//      cout << img_coords.x_cen << " " << img_coords.y_cen << " " << img_coords.x_min << " " << img_coords.y_min << " " << img_coords.x_max << " " << img_coords.y_max << endl;
//
//    }


  }
}


void AnnotationReader::getNextNumberInString(string& line, int curr_pos, int& number, int& next_index)
{
  int number_ind = curr_pos + line.substr(curr_pos,-1).find_first_of("0123456789");
  next_index = number_ind + line.substr(number_ind, -1).find_first_of(",(");

  number = atoi(line.substr(number_ind, -1).c_str());


}

void AnnotationReader::readAllAnnotations()
{

}
