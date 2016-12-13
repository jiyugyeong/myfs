#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
typedef struct inode_st{
	char type;
	short direct_block;
	struct tm c_time;
	int size;
	short indirect_block;
	short double_indirect_block;
}inode_st;
typedef struct dfile{
  char file_name[4];
  short file_inode;
}dfile;
typedef struct ram_dfile{
  char file_name[5]; //null값을 위해 5칸
  short file_inode;
}ram_dfile;
typedef struct regular{
	unsigned char storage[128];
}regular;
typedef struct directory{
  short parent;
  short present;
  int famount;
  dfile name_inode[20];
}directory;
typedef union data_un{
  directory dir;
  regular reg;
}data_un;
typedef struct file_system{
	char boot[2];
	unsigned char sup_inode[64];
	unsigned char sup_data[128];
	inode_st inode[512];
	data_un data[1024];
}file_system;
typedef struct tree{
  char file_name[5];
  int file_inode;
  struct tree *parents;
  struct tree *siblings;
  struct tree *children;
}tree;

int count_empty_block(unsigned char *check,int length);
int *list_data_block(int num,file_system *mf);
void mystate(char *order,file_system *mf);
char *route_to_name(char *course);
int *list_empty_db(unsigned char *check,int size_c);
void write_fname_finode(file_system *mf,char *myf_name,int i_empty,int file_inode);
int indirect_bit_to_int(unsigned char *storage,int num);
void indirect_int_to_bit(unsigned char *storage,short num);
int cal_indirect_num(unsigned char *storage);
tree *create_tree(char *file_name,int file_inode);
int func_order(char *order,FILE *fop,file_system *mf,tree *root,tree *tnow,tree **n2ow);
void mycpfrom(char *order,file_system *mf);
void mark_super(unsigned char *check,int num);
void change_fname_finode(file_system *mf,char *myf_name,char *change_name,int work_station);
void remove_fname_finode(file_system *mf,char *myf_name,int work_station);
void reset_data_block(int num,file_system *mf);
void reset_indirect(int num,file_system *mf);
void myshowinode(char *order,file_system *mf);
int count_data_block(int num,file_system *mf);
void myshowfile(char *order,file_system *mf);
void mypwd(char *order,file_system *mf);
void mycd(char *order,file_system *mf,tree *root,tree **n2ow);
int find_route(char *course,file_system *mf);
void mymkdir(char *order,file_system *mf);
void unmark_super(unsigned char *check,int num);
int find_empty_block(unsigned char *ch,int length);
void mymv(char *order,file_system *mf);
void recursion_tree(file_system *mf,tree *work_station);
tree *change_work_station(char (*name)[5],int num,tree *root,tree *now);
void recursion_print(int deep,tree *work_station);
void print_tree(char *order,tree *root,tree *now,file_system *mf);
int find_file(directory storage,char *name);
int cal_block(int file_size);
void myls(char *order,file_system *mf);
void time_to_string(struct tm ct,char *ct_s);
int sort_str(const void *v1,const void *v2);
void mycat(char *order,file_system *mf);
void myshowblock(char *order,file_system *mf);
void mytouch(char *order,file_system *mf);
void remove_indirect_bit(unsigned char *storage,short count,file_system *mf);
void mycpto(char *order,file_system *mf);
void myrmdir(char *order,file_system *mf);
void myrm(char *order,file_system *mf);


char now_directory[10000]="/";
int now=0;

int main(){
  FILE *fop=fopen("myfs","rb");
  char order[1000];
  tree *root,*tnow;
  root=create_tree("/",0);
  root->parents=root;
  tnow=root;
  if(fop==NULL){
		printf("오류 : 파일이 없습니다.\n");
		return 1;
	}
  file_system mf;

  fread(&mf,sizeof(mf),1,fop);
  while(1)
  {
    printf("[%s ]$ ",now_directory);
    fgets(order,sizeof(order),stdin);
    recursion_tree(&mf,root);
    if(func_order(order,fop,&mf,root,tnow,&tnow)==-1)
      break;
  }
  fclose(fop);
  fop=fopen("myfs","wb");
  fwrite(&mf,sizeof(mf),1,fop);
  fclose(fop);
  return 0;
}
int func_order(char *order,FILE *fop,file_system *mf,tree *root,tree *tnow,tree **n2ow){
	if(strncmp(order,"byebye",6)==0)
		return -1;
	else if(strncmp(order,"my",2)!=0)
		system(order);
	else if(strncmp(order,"mycpfrom",8)==0)
		mycpfrom(order,mf);
	else if(strncmp(order,"myls",4)==0)
		myls(order,mf);
  else if(strncmp(order,"mycat",5)==0)
  	mycat(order,mf);
  else if(strncmp(order,"mystate",7)==0)
    mystate(order,mf);
  else if(strncmp(order,"myshowfile",10)==0)
    myshowfile(order,mf);
  else if(strncmp(order,"mypwd",5)==0)
    mypwd(order,mf);
  else if(strncmp(order,"mycd",4)==0)
    mycd(order,mf,root,n2ow);
  else if(strncmp(order,"mymkdir",7)==0)
    mymkdir(order,mf);
  else if(strncmp(order,"myshowinode",11)==0)
    myshowinode(order,mf);
  else if(strncmp(order,"myshowblock",11)==0)
    myshowblock(order,mf);
  else if(strncmp(order,"mytouch",7)==0)
    mytouch(order,mf);
  else if(strncmp(order,"mycpto",6)==0)
    mycpto(order,mf);
  else if(strncmp(order,"myrmdir",7)==0)
    myrmdir(order,mf);
  else if(strncmp(order,"myrm",4)==0)
    myrm(order,mf);
  else if(strncmp(order,"mymv",4)==0)
    mymv(order,mf);
    else if(strncmp(order,"mytree",6)==0)
      print_tree(order,root,tnow,mf);
  else
    printf("오류 : 명령이 없습니다.\n");
	return 0;
}
void mycat(char *order,file_system *mf){
  char f1name[1000]="",f2name[1000]="",f3name[1000]="",f4name[1000]="",*name;
  short work_station;
  int rn,rn2,rn3,*data_list,*dnum2,*dnum3,fin,file_size,temp,*str,new_inode;
  time_t now_time;
  now_time=time(NULL);
  sscanf(order,"%*s %s %s %s %s",f1name,f2name,f3name,f4name);

  if(f1name[0]!=0&&f2name[0]==0&&f3name[0]==0&&f4name[0]==0){
    work_station=find_route(f1name,mf);
    fin=work_station;
    if(fin==-1){
      printf("오류 : 파일이 없습니다. '%s'\n",f1name);
      return ;
    }
    if(mf->inode[fin].type=='d'){
      printf("오류 : directory 파일 입니다. '%s'\n",f1name);
      return ;
    }
    file_size=mf->inode[fin].size;
    temp=file_size;
    str=list_data_block(fin,mf);
    rn=count_data_block(fin,mf);
    for(int i=0;i<rn;i++){
      for(int j=0;j<128;j++)
        printf("%c",mf->data[str[i]].reg.storage[j]);
    }
  }
  if(f1name[0]!=0&&f2name[0]!=0&&f3name[0]==0&&f4name[0]==0){
    work_station=find_route(f1name,mf);
    fin=work_station;
    if(fin==-1){
      printf("오류 : 파일이 없습니다.'%s'\n",f1name);
      return ;
    }
      file_size=mf->inode[fin].size;
      temp=file_size;
      str=list_data_block(fin,mf);
      rn=count_data_block(fin,mf);
      for(int i=0;i<rn;i++){
        if(file_size-128>=0){
          temp=128;
          file_size=file_size-128;
        }
        else
          temp=file_size;
        for(int j=0;j<temp;j++)
          printf("%c",mf->data[str[i]].reg.storage[j]);
      }
      free(str);
      work_station=find_route(f2name,mf);
      fin=work_station;
      if(fin==-1){
        printf("오류 : 파일이 없습니다.'%s'\n",f2name);
        return ;
      }
        file_size=mf->inode[fin].size;
        temp=file_size;
        str=list_data_block(fin,mf);
        rn=count_data_block(fin,mf);
        for(int i=0;i<rn;i++){
          if(file_size-128>=0){
            temp=128;
            file_size=file_size-128;
          }
          else
            temp=file_size;
          for(int j=0;j<temp;j++)
            printf("%c",mf->data[str[i]].reg.storage[j]);
        }
        free(str);
  }
  if(f1name[0]!=0&&f2name[0]!=0&&f3name[0]=='>'&&f4name[0]!=0){
    if(find_route(f1name,mf)==-1 ){
      printf("오류 : 파일이 없습니다.'%s'\n",f1name);
      return ;
    }
    if(find_route(f2name,mf)==-1 ){
      printf("오류 : 파일이 없습니다.'%s'\n",f2name);
      return ;
    }
    if(mf->inode[find_route(f1name,mf)].type=='d'){
      printf("오류 : directory 파일 입니다.\n");
      return ;
    }
    if(mf->inode[find_route(f2name,mf)].type=='d'){
      printf("오류 : directory 파일 입니다.\n");
      return ;
    }
    if(find_route(f1name,mf)==find_route(f3name,mf)){
      printf("오류 : 출력과 입력 파일이 같습니다.\n");
      return ;
    }
    if(find_route(f2name,mf)==find_route(f3name,mf)){
      printf("오류 : 출력과 입력 파일이 같습니다.\n");
      return ;
    }
    data_list=list_data_block(find_route(f1name,mf),mf);
    dnum2=list_data_block(find_route(f2name,mf),mf);
    rn=count_data_block(find_route(f1name,mf),mf);
    rn2=count_data_block(find_route(f2name,mf),mf);
    if(find_route(f4name,mf)==-1){
      int size_c;
      new_inode=find_empty_block(mf->sup_inode,64);
      mark_super(mf->sup_inode,new_inode);
      file_size=mf->inode[find_route(f1name,mf)].size+mf->inode[find_route(f2name,mf)].size;
      size_c=cal_block(file_size);
      str=list_empty_db(mf->sup_data,size_c);
      mf->inode[new_inode].type='-';
      mf->inode[new_inode].c_time=*(localtime(&now_time));
      mf->inode[new_inode].size=mf->inode[find_route(f1name,mf)].size+mf->inode[find_route(f2name,mf)].size;
      mf->inode[new_inode].direct_block=str[0];
      mf->inode[new_inode].indirect_block=str[size_c-1];
      mf->inode[new_inode].double_indirect_block=-1;
      for(int i=1;i<size_c-1;i++){
        indirect_int_to_bit(mf->data[mf->inode[new_inode].indirect_block].reg.storage,str[i]);
      }
      dnum3=list_data_block(new_inode,mf);
      rn3=count_data_block(new_inode,mf);
      int pos=0;
      for(int i=0;i<rn;i++){
        if(i==rn-1){
          for(int j=0;j<mf->inode[find_route(f1name,mf)].size-128*(rn-1);j++){
            mf->data[dnum3[pos/128]].reg.storage[pos%128]=mf->data[data_list[i]].reg.storage[j];
            pos++;
          }
        }
        else
          for(int j=0;j<128;j++){
            mf->data[dnum3[pos/128]].reg.storage[pos%128]=mf->data[data_list[i]].reg.storage[j];
            pos++;
          }
      }
      for(int i=0;i<rn2;i++){
        if(i==rn2-1){
          for(int j=0;j<mf->inode[find_route(f2name,mf)].size-128*(rn2-1);j++){
            mf->data[dnum3[pos/128]].reg.storage[pos%128]=mf->data[dnum2[i]].reg.storage[j];
            pos++;
          }
        }
        else
          for(int j=0;j<128;j++){
            mf->data[dnum3[pos/128]].reg.storage[pos%128]=mf->data[dnum2[i]].reg.storage[j];
            pos++;
          }
      }
      name=route_to_name(f4name);
      if(name==NULL) {name=f4name; work_station=now;}
      else if(name==(char *)1) {name=f4name+1; work_station=0;}
      else  {work_station=find_route(f4name,mf);}
      write_fname_finode(mf,name,new_inode,work_station);
      free(dnum3);
      free(str);
    }
    else if(find_route(f4name,mf)!=-1){
      dnum3=list_data_block(find_route(f4name,mf),mf);
      rn3=count_data_block(find_route(f4name,mf),mf);
      if(rn3==1){
        mf->inode[find_route(f4name,mf)].indirect_block=find_empty_block(mf->sup_data,128);
        mark_super(mf->sup_data,mf->inode[find_route(f4name,mf)].indirect_block);
        str=list_empty_db(mf->sup_data,rn+rn2-1);
        for(int i=0;i<rn+rn2-1;i++)
          indirect_int_to_bit(mf->data[mf->inode[find_route(f4name,mf)].indirect_block].reg.storage,str[i]);
        free(str);
      }
      else if(rn3>rn+rn2){
        remove_indirect_bit(mf->data[mf->inode[find_route(f4name,mf)].indirect_block].reg.storage,rn3-rn-rn2,mf);
      }
      else if(rn3<rn+rn2){
        str=list_empty_db(mf->sup_data,rn+rn2-rn3);
        for(int i=0;i<rn+rn2-rn3;i++)
          indirect_int_to_bit(mf->data[mf->inode[find_route(f4name,mf)].indirect_block].reg.storage,str[i]);
        free(str);
      }
      mf->inode[find_route(f4name,mf)].c_time=*(localtime(&now_time));
      mf->inode[find_route(f4name,mf)].size=mf->inode[find_route(f1name,mf)].size+mf->inode[find_route(f2name,mf)].size;
      dnum3=list_data_block(find_route(f4name,mf),mf);
      for(int i=0;i<rn;i++){
        for(int j=0;j<128;j++)
          mf->data[dnum3[i]].reg.storage[j]=mf->data[data_list[i]].reg.storage[j];
      }
      for(int i=0;i<rn2;i++){
        for(int j=0;j<128;j++)
          mf->data[dnum3[i+rn]].reg.storage[j]=mf->data[dnum2[i]].reg.storage[j];
      }
      free(dnum3);
