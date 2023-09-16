 #include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
typedef FILE image;
int main(void)
{
    image *img1 = NULL,*img2 = NULL;
    int value = 0;
    img1 = fopen("img.png","rb");
    img2 = fopen("hhh.png","wb");
   
    if(img1 == NULL)
    {
        perror("File Not Found");
        return EXIT_FAILURE;
    }
    while((value=getw(img1))!=EOF)
    {
        fwrite(&value,4,1,img2);
    }
   
    fclose(img1);
    fclose(img2);
    return EXIT_SUCCESS;
}
