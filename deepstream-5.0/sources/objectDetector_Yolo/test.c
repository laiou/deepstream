#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 2048


void split(char *src,const char *separator,char **dest,int *num) {

     char *pNext=NULL;
     int count = 0;
     if (src == NULL || strlen(src) == 0) 
        return;
     if (separator == NULL || strlen(separator) == 0) 
        return;
     pNext = (char *)strtok(src,separator); 
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator);  
    }
    *num = count;
}

int main()
{
 char arr[1000];
 char str[100]; 
 char buf[MAX_LINE];  
 char buf1[MAX_LINE];
 int num1 = 0;
 char *revbuf1[100] = {NULL};
 FILE *fp=NULL;            
 FILE *fp1 = NULL;
 FILE *fp2 = NULL;
 char width[]="123456";
 char height[]="123456";
 char row[]="123456";
 char columns[]="123456";
 int num = 0;
 char *revbuf[100] = {NULL};
 int len;            
 char  string1[] = "input_width";
 char  string2[] = "input_height";
 char  string3[] = "show_row";
 char  string4[] = "show_columns";
 char  string5[] = "file";
 char  string6[] = "rtsp";
 char  string7[] = "usb";
 int x = 0;
 int i = 0;
 if((fp = fopen("./source.txt","r")) == NULL)
 {
 perror("fail to read");
 exit (1) ;
 }


 fp1 = fopen("./deepstream_config.txt","a+");
 fputs("[ds-example]\n",fp1);
 fputs("enable=1\n",fp1);
 fputs("processing-width=640\n",fp1);
 fputs("processing-height=480\n",fp1);
 fputs("full-frame=0\n",fp1);
 fputs("unique-id=15\n",fp1);
 fputs("gpu-id=0\n",fp1);
 fputs("[application]\n",fp1);
 fputs("enable-perf-measurement=1\n",fp1);
 fputs("perf-measurement-interval-sec=5\n",fp1);
 fputs("[tiled-display]\n",fp1);
 fputs("enable=2\n",fp1);
 
//char a[] = "1234567";

 while(fgets(buf,MAX_LINE,fp) != NULL)
 {
 len = strlen(buf);
// buf[len-1] = '\0';
 split(buf,",",revbuf,&num);
 

 if(strcmp(revbuf[0], string1) == 0){
	 //width = revbuf[1];
	 strcpy(width,revbuf[1]);
         }
 else if(strcmp(revbuf[0],string2)==0){
	 //height = revbuf[1];
	 strcpy(height,revbuf[1]);}
 else if(strcmp(revbuf[0],string3)==0){
	 //row = revbuf[1];
	 strcpy(row,revbuf[1]);}
 else if(strcmp(revbuf[0],string4)==0){
	 //columns = revbuf[1];
	 strcpy(columns,revbuf[1]);}


printf("%s \n",revbuf[0]);
printf("%s \n",revbuf[1]);
 
 }

fclose(fp);
fputs("rows=3",fp1);
//printf("qqq\n");
//printf("%s \n",width);
//fputs(row,fp1);
fputs("\n",fp1);
fputs("columns=3",fp1);
//fputs(columns,fp1);
fputs("\n",fp1);
fputs("width=1280",fp1);
//fputs(width,fp1);
fputs("\n",fp1);
fputs("height=720",fp1);
//fputs(height,fp1);
fputs("\n",fp1);
fputs("gpu-id=0\n",fp1);
fputs("nvbuf-memory-type=0\n",fp1);
//printf("lalal\n");
//fp2 = fopen("bb.txt","r");
if((fp2 = fopen("./source.txt","r")) == NULL)
 {
 perror("fail to read");
 exit (1) ;
 }
//printf("enenen\n");
while(fgets(buf1,MAX_LINE,fp2) != NULL)
 {
//printf("jinle\n");
 len = strlen(buf1);
// buf[len-1] = '\0';  /*去掉换行符*/
 split(buf1,",",revbuf1,&num1);
//printf("dangdang\n");
//string5-->file
 if(strcmp(revbuf1[0], string5) == 0){
         //width = revbuf[1];
	 //printf("yeye\n");
         fputs("[source",fp1);
	 sprintf(str,"%d",x);
	 fputs(str,fp1);
	 fputs("]",fp1);
	 fputs("\n",fp1);
	 fputs("enable=1\n",fp1);
	 fputs("type=3\n",fp1);
 	 fputs("uri=file://",fp1);
	 fputs(revbuf1[1],fp1);
	// fputs("\n",fp1);
         fputs("num-sources=1\n",fp1);
	 fputs("gpu-id=0\n",fp1);
	 fputs("cudadec-memtype=0\n",fp1);
	 x = x+1;	 
         }
 else if (strcmp(revbuf1[0], string6) == 0){
 	 //printf("wwww\n");
	 fputs("[source",fp1);
	 sprintf(str,"%d",x);
         fputs(str,fp1);
         fputs("]",fp1);
         fputs("\n",fp1);
         fputs("enable=1\n",fp1);
	 fputs("type=3\n",fp1);
         fputs("uri=rtsp://",fp1);
         fputs(revbuf1[1],fp1);
         fputs("\n",fp1);
         fputs("num-sources=1\n",fp1);
         fputs("gpu-id=0\n",fp1);
         fputs("cudadec-memtype=0\n",fp1);
         x = x+1;

 }
 else if (strcmp(revbuf1[0],string7)==0){
 	fputs("[source",fp1);
	sprintf(str,"%d",x);
	fputs(str,fp1);
	fputs("]",fp1);
	fputs("\n",fp1);
	fputs("enable=1\n",fp1);
	fputs("type=1\n",fp1);
	fputs("camera-width=",fp1);
	fputs(width,fp1);
	fputs("\n",fp1);
	fputs("camera-height=",fp1);
	fputs(height,fp1);
	fputs("\n",fp1);
	fputs("camera-fps-n=30\n",fp1);
	fputs("camera-fps-d=1\n",fp1);
	fputs("camera-v4l2-dev-node=0\n",fp1);
	fputs("num-sources=1\n",fp1);
	fputs("gpu-id=0\n",fp1);
	fputs("cudadec-memtype=0\n",fp1);
	x = x + 1;
 }
 

 }

fclose(fp2);

for (i = 0; i < x; i++){
    fputs("[sink",fp1);
    sprintf(str,"%d",i);
    fputs(str,fp1);
    fputs("]\n",fp1);
    fputs("enable=1\n",fp1);
    fputs("type=8\n",fp1);
    fputs("container=1\n",fp1);
    fputs("codec=1\n",fp1);
    fputs("bitrate=5000000\n",fp1);
    fputs("sync=0\n",fp1);
    fputs("source-id=",fp1);
    fputs(str,fp1);
    fputs("\n",fp1);
    fputs("gpu-id=0\n",fp1);
    fputs("output-file=/mnt/hls_",fp1);
    fputs(str,fp1);
    fputs("\n",fp1);
    fputs("nvbuf-memory-type=0\n",fp1);
    fputs("link-to-demux=1\n",fp1);
}

fputs("[osd]\n",fp1);
fputs("enable=1\n",fp1);
fputs("gpu-id=0\n",fp1);
fputs("border-width=1\n",fp1);
fputs("text-size=15\n",fp1);
fputs("text-color=0.3;0.3;0.3;1\n",fp1);
fputs("text-bg-color=1;1;1;1;\n",fp1);
fputs("font=Serif\n",fp1);
fputs("show-clock=0\n",fp1);
fputs("clock-x-offset=800\n",fp1);
fputs("clock-y-offset=820\n",fp1);
fputs("clock-text-size=12\n",fp1);
fputs("clock-color=1;0;0;0\n",fp1);
fputs("nvbuf-memory-type=0\n",fp1);
fputs("[streammux]\n",fp1);
fputs("gpu-id = 0\n",fp1);
fputs("live-source = 1\n",fp1);
fputs("batch-size =",fp1);
sprintf(str,"%d",x);
fputs(str,fp1);
fputs("\n",fp1);
fputs("batched-push-timeout = 40000\n",fp1);
fputs("width =1280",fp1);
//fputs(width,fp1);
fputs("\n",fp1);


fputs("height =720",fp1);
//fputs(height,fp1);
fputs("\n",fp1);


fputs("enable-padding = 0\n",fp1);
fputs("nvbuf-memory-type = 0\n",fp1);

fputs("[primary-gie]\n",fp1);
fputs("enable = 1\n",fp1);
fputs("gpu-id = 0\n",fp1);
fputs("model-engine-file=/opt/nvidia/deepstream/deepstream-5.0/sources/objectDetector_Yolo/model_b9_gpu0_int8.engine\n",fp1);
fputs("labelfile-path =/opt/nvidia/deepstream/deepstream-5.0/sources/objectDetector_Yolo/labels-voc.txt\n",fp1);
fputs("batch-size =",fp1);
fputs(str,fp1);
fputs("\n",fp1);
fputs("bbox-border-color0 = 1;0;0;1\n",fp1);
fputs("bbox-border-color1 = 0;1;1;1\n",fp1);
fputs("bbox-border-color2 = 0;0;1;1\n",fp1);
fputs("bbox-border-color3 = 0;1;0;1\n",fp1);
fputs("interval = 0\n",fp1);
fputs("gie-unique-id = 1\n",fp1);
fputs("nvbuf-memory-type = 0\n",fp1);
fputs("config-file =/opt/nvidia/deepstream/deepstream-5.0/sources/objectDetector_Yolo/config_infer_primary_yoloV3.txt\n",fp1);
fputs("[tests]\n",fp1);
fputs("file-loop = 1\n",fp1);


  fclose(fp1);
  return 0;
}
