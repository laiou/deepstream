# include<stdlib.h>
# include<stdio.h>
# include<unistd.h>
main()
{

int i  = 1;
int j = 1;
int a, b, c, d;
if (access("/dev/xdma0_user", 4))
{
    printf("Fatal Error : No permission to access the /dev/xmda*. Please try 'sudo chmod 777 /dev/xdma*.'\n");
_exit(0);
}

if (access("../../project", 4)){
printf("Fatal Error : No permission to access the project. Please try 'sudo chmod 777 project.'\n");
_exit(0);
}

a =  system("../tools/reg_c /dev/xdma0_user 0xc ");
b =  system("../tools/reg_4 /dev/xdma0_user 0x4 ");
if (WIFEXITED(a)){
// printf("0xc = %d\n", WEXITSTATUS(a));
if (WEXITSTATUS(a) == 0){
printf("Fatal Error : Board not connect. Default value is wrong.\n");
_exit(0);
}
}
if (WIFEXITED(b)){
// printf("0x4 = %d\n", WEXITSTATUS(b));
if (WEXITSTATUS(b) == 0){
printf("Fatal Error : Optical module is not connect.\n");
_exit(0);
}
}
if (access("/dev/xdma0_user", 0)){
printf("Fatal Error : Board not connect. Not found /dev/xdma.\n");
_exit(0);
}



if (i == 1){
//system("../tools/test");

//get infer_config_2.txt
sleep(1);

system("../tools/encode ../model/infer_config_2.txt -");
// sleep(1);
system("../tools/encode ../model/model_infer.txt -");
// sleep(1);
system("../tools/encode ../model/model.cfg -");
// system("../tools/encode ../model/labels.cfg -");
sleep(1);
system("../tools/encode ../tools/optical-helmet-demo -");
// system("../tools/optical-helmet-demo -c ../model/infer_config_2.txt");
system("nohup  ../tools/optical-helmet-demo -c ../model/infer_config_2.txt >/dev/null 2>&1 &");

// system("deepstream-app -c deepstream_app_config_yoloV3_video.txt &");
sleep(1);
system("../tools/encode ../tools/optical-helmet-demo +");
system("../tools/encode ../model/infer_config_2.txt +");
system("rm -rf ../model/infer_config_2.txt");
// sleep(1);
system("../tools/encode ../model/model_infer.txt +");
// sleep(1);
system("../tools/encode ../model/model.cfg +");
// system("../tools/encode ../model/labels.cfg +");

printf("Now start checking...\n");

while (i == 1){
printf("done.\n");
a =  system("../tools/reg_c /dev/xdma0_user 0xc");
b =  system("../tools/reg_4 /dev/xdma0_user 0x4");

if (WIFEXITED(a)){
// printf("0xc = %d\n", WEXITSTATUS(a));
if (WEXITSTATUS(a) == 0){
printf("Fatal Error : Board not connect. Default value is wrong.\n");
system("killall -9 optical-helmet-demo");
system("rm -rf  ../model/model_infer.txt ../model/infer_config_2.txt ../model/model.cfg ../model/labels.cfg");
system("cp ../tools/infer_config_2.txt.bak ../model");
system("cp ../tools/model_infer.txt.bak ../model");
system("cp ../tools/model.cfg.bak ../model");
system("cp ../tools/labels.cfg.bak ../model");
system("mv ../model/infer_config_2.txt.bak ../model/infer_config_2.txt");
system("mv ../model/model_infer.txt.bak ../model/model_infer.txt");
system("mv ../model/model.cfg.bak ../model/model.cfg");
system("mv ../model/labels.cfg.bak ../model/labels.cfg");
_exit(0);
}
}
if (WIFEXITED(b)){
// printf("0x4 = %d\n", WEXITSTATUS(b));
if (WEXITSTATUS(b) == 0){
printf("Fatal Error : Optical module is not connect.\n");
system("killall -9 optical-helmet-demo");
system("rm -rf  ../model/model_infer.txt ../model/model.cfg ../model/labels.cfg");
system("cp ../tools/infer_config_2.txt.bak ../model");
system("cp ../tools/model_infer.txt.bak ../model");
system("cp ../tools/model.cfg.bak ../model");
system("cp ../tools/labels.cfg.bak ../model");
system("mv ../model/infer_config_2.txt.bak ../model/infer_config_2.txt");
system("mv ../model/model_infer.txt.bak ../model/model_infer.txt");
system("mv ../model/model.cfg.bak ../model/model.cfg");
system("mv ../model/labels.cfg.bak ../model/labels.cfg");
_exit(0);
}
}
if (access("/dev/xdma0_user", 0)){
printf("Fatal Error : Board not connect. Not found /dev/xdma.\n");
system("killall -9 optical-helmet-demo");
system("rm -rf  ../model/model_infer.txt ../model/model.cfg ../model/labels.cfg");
system("cp ../tools/infer_config_2.txt.bak ../model");
system("cp ../tools/model_infer.txt.bak ../model");
system("cp ../tools/model.cfg.bak ../model");
system("cp ../tools/labels.cfg.bak ../model");
system("mv ../model/infer_config_2.txt.bak ../model/infer_config_2.txt");
system("mv ../model/model_infer.txt.bak ../model/model_infer.txt");
system("mv ../model/model.cfg.bak ../model/model.cfg");
system("mv ../model/labels.cfg.bak ../model/labels.cfg");
_exit(0);
}
if (access("/dev/xdma0_user", 4)){
printf("Fatal Error : No permission to access the /dev/xmda*. Please try 'sudo chmod 777 /dev/xdma*.'\n");
system("killall -9 optical-helmet-demo");
system("rm -rf  ../model/model_infer.txt ../model/model.cfg ../model/labels.cfg");
system("cp ../tools/infer_config_2.txt.bak ../model");
system("cp ../tools/model_infer.txt.bak ../model");
system("cp ../tools/model.cfg.bak ../model");
system("cp ../tools/labels.cfg.bak ../model");
system("mv ../model/infer_config_2.txt.bak ../model/infer_config_2.txt");
system("mv ../model/model_infer.txt.bak ../model/model_infer.txt");
system("mv ../model/model.cfg.bak ../model/model.cfg");
system("mv ../model/labels.cfg.bak ../model/labels.cfg");
_exit(0);
}



sleep(1);

} // while

} // i == 1

} // main






