/***************************** Include Files *********************************/
#include "bram_uio.hpp"
#include <iostream>
#define IMG_H 10
#define IMG_W 10

#define BYTES_PR_PX 4
/************************** Function Implementation *************************/

BRAM::BRAM(unsigned int uio_number, unsigned int size)
{
    char device_file_name[20];
    sprintf(device_file_name, "/dev/uio%d", uio_number);

    int device_file;

    if ((device_file = open(device_file_name, O_RDWR | O_SYNC)) < 0)
    {
        std::stringstream ss;
        ss << device_file_name << " could not be opened";
        throw ss.str();
    }

    bram_ptr = (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, device_file, 0x0);
    
    if (bram_ptr == NULL)
    {
        std::stringstream ss;
        ss << "Could not map memory";
        throw ss.str();
    }
}

void BRAM::pic2bram(cv::Mat &img)
{
    bram_ptr[128] = 4;
    int counter = 0;
    char tmp[BYTES_PR_PX];
    for (int i = 0; i < IMG_W; i++)
    {
        for (int j = 0; j < IMG_H; j++)
        {
            float px = img.at<float>(i, j);
            memcpy(tmp, &px, BYTES_PR_PX);
            uint32_t val = (uint32_t)((tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | (tmp[0]));
            bram_ptr[counter] = val;
            counter++;
        }
    }
}

uint32_t &BRAM::operator[](unsigned int index)
{
    return bram_ptr[index];
}

uint32_t *BRAM::GetPointer()
{
    return bram_ptr;
}
