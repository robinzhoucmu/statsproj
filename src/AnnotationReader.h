#include <iostream>
#include <fstream>
#include <string>
#include <vector>


struct Img_Coordinates
{
  int x_cen;
  int y_cen;
  int x_min;
  int y_min;
  int x_max;
  int y_max;
};

struct Image_Info
{
  std::string image_name;
  std::vector<Img_Coordinates> people_coordinates;
  
};


class AnnotationReader
{
  public:
    AnnotationReader(const std::string& directory, const std::string& folder);


    void readAllAnnotations();

    std::vector <Image_Info> _images_info;

  private:
    void getNextNumberInString(std::string& line, int curr_pos, int& number, int& next_index);

};
