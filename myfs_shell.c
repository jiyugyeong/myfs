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
    }
    free(data_list);
    free(dnum2);
  }
}
void myshowinode(char *order,file_system *mf){
int num;
char ct_s[20];
  sscanf(order,"%*s %d",&num);
if(mf->inode[num-1].type=='d')
    printf("file type : directory file\n");
elseif(mf->inode[num-1].type==0){
    printf("오류 : 비어있는 inode '%d'\n",num);
return;
  }
elseif(mf->inode[num-1].type=='-')
    printf("file type : regular file\n");
  printf("file size : %d byte\n",mf->inode[num-1].size);
  time_to_string(mf->inode[num-1].c_time,ct_s);
  printf("modified time : %s\n",ct_s);
  printf("data block list : %d, %d, %d\n",mf->inode[num-1].direct_block+1,mf->inode[num-1].indirect_block+1,mf->inode[num-1].double_indirect_block+1);
}

void reset_indirect(intnum,file_system *mf){
structtm temp={0};
mf->inode[num].type=0;
mf->inode[num].c_time=temp;
mf->inode[num].size=0;
mf->inode[num].direct_block=0;
mf->inode[num].indirect_block=0;
mf->inode[num].double_indirect_block=0;
}
char *route_to_name(char *course){
char dname[5]="",temp[1000]="",*name;
if(course[0]=='/'){
int count=1,i=1;
while(1)
     {
if(course[2]==0)
        {
           count--;
break;
        }
if(course[i]=='/'&&course[i+1]!=0)
           count++;
elseif(course[i]=='/'&&course[i+1]==0)
break;
if(course[i] == 0)
break;
        i++;
     }
if(count!=1){
for(int i=strlen(course)-1,j=0;;i--,j++){
if(course[i]=='/')
break;
else{
          temp[j]=course[i];
course[i]=0;
        }
      }
for(int i=0;i<4;i++)
        dname[i]=temp[strlen(temp)-1-i];
      }
else{
         strncpy(dname,course+1,4);
return (char *)1;
      }
  }
else{
int count=0;
if(course[strlen(course)-1]=='/')
course[strlen(course)-1]=0;
for(int i=0;i<=(int)strlen(course);i++){
if(course[i]=='/'||course[i]==0){
        count++;
      }
    }
if(count!=1){
for(int i=strlen(course)-1,j=0;;i--,j++){
if(course[i]=='/')
break;
else{
         temp[j]=course[i];
course[i]=0;
       }
     }
for(int i=0;i<4;i++)
       dname[i]=temp[strlen(temp)-1-i];
    }
else {
       strncpy(dname,course,4);
returnNULL;
    }
  }
  name=(char *)calloc(strlen(dname)+1,sizeof(char));
  strncpy(name,dname,strlen(dname)+1);
return name;
}
int find_route(char *course,file_system *mf){
char bck[1000],(*nlist)[5],*temp;
int i=1,fin,rn,*data_list;
  strcpy(bck,course);
if(course[0] == '/'){
int count=1;
while(1)
       {
if(course[2]==0)
          {
             count--;
break;
          }
if(course[i]=='/'&&course[i+1]!=0)
             count++;
elseif(course[i]=='/'&&course[i+1]==0)
break;
if(course[i] == 0)
break;
          i++;
       }
if(count==0)
return 0;
       nlist=(char (*)[5])calloc(count*5,sizeof(char));
       temp=strtok(bck,"/");
       nlist[0][0]=temp[0];
       nlist[0][1]=temp[1];
       nlist[0][2]=temp[2];
       nlist[0][3]=temp[3];
for(int i=1;i<count;i++){
         temp=strtok(NULL,"/");
         nlist[i][0]=temp[0];
         nlist[i][1]=temp[1];
         nlist[i][2]=temp[2];
         nlist[i][3]=temp[3];
       }
       fin=0;
       data_list=list_data_block(fin,mf);
       rn=count_data_block(fin,mf);
if(strncmp(nlist[0],"..",2)==0)
         fin=mf->data[data_list[0]].dir.parent;
elseif(strncmp(nlist[0],".",2)==0)
         fin=mf->data[data_list[0]].dir.present;
else{
for(int i=0;i<rn;i++){
         fin=find_file(mf->data[data_list[i]].dir,nlist[0]);
if(fin!=-1) break;
       }
if(fin==-1){
return -1;
       }
       free(data_list);
     }
for(int i=1;i<count;i++){
         data_list=list_data_block(fin,mf);
         rn=count_data_block(fin,mf);
if(strncmp(nlist[i],"..",2)==0)
           fin=mf->data[data_list[i]].dir.parent;
elseif(strncmp(nlist[0],".",2)==0)
           fin=mf->data[data_list[0]].dir.present;
else{
for(int j=0;j<rn;j++){
           fin=find_file(mf->data[data_list[j]].dir,nlist[i]);
if(fin!=-1) break;
          }
if(fin==-1){
return -1;
              }
            free(data_list);
          }
        }
     }
else{
int count=0;
if(course[strlen(course)-1]=='/')
course[strlen(course)-1]=0;
for(int i=0;i<=(int)strlen(course);i++){
if(course[i]=='/'||course[i]==0){
          count++;
        }
      }
      nlist=(char (*)[5])calloc(count*5,sizeof(char));
      temp=strtok(bck,"/");
      nlist[0][0]=temp[0];
      nlist[0][1]=temp[1];
      nlist[0][2]=temp[2];
      nlist[0][3]=temp[3];
for(int i=1;i<count;i++){
        temp=strtok(NULL,"/");
        nlist[i][0]=temp[0];
        nlist[i][1]=temp[1];
        nlist[i][2]=temp[2];
        nlist[i][3]=temp[3];
      }
      fin=now;
      data_list=list_data_block(fin,mf);
      rn=count_data_block(fin,mf);
if(strncmp(nlist[0],"..",2)==0)
        fin=mf->data[data_list[0]].dir.parent;
elseif(strncmp(nlist[0],".",2)==0)
        fin=mf->data[data_list[0]].dir.present;
else
      {
for(int i=0;i<rn;i++){
          fin=find_file(mf->data[data_list[i]].dir,nlist[0]);
if(fin!=-1) break;
        }
if(fin==-1){
return -1;
        }
      }
      free(data_list);
for(int i=1;i<count;i++){
        data_list=list_data_block(fin,mf);
        rn=count_data_block(fin,mf);
if(strncmp(nlist[i],"..",2)==0)
          fin=mf->data[data_list[i]].dir.parent;
elseif(strncmp(nlist[0],".",2)==0)
          fin=mf->data[data_list[0]].dir.present;
else
        {
for(int j=0;j<rn;j++){
            fin=find_file(mf->data[data_list[j]].dir,nlist[i]);
if(fin!=-1) break;
            }
if(fin==-1){
return -1;
            }
            free(data_list);
          }
        }
    }
return fin;
    free(nlist);
}
int *list_data_block(intnum,file_system *mf){
int rn,*dstr,data_list;
  rn=count_data_block(num,mf);
  dstr=(int *)calloc(rn,sizeof(int));
  dstr[0]=mf->inode[num].direct_block;
  data_list=mf->inode[num].indirect_block;
for(int i=1;i<rn;i++)
    dstr[i]=indirect_bit_to_int(mf->data[data_list].reg.storage,i);
return dstr;
}
int count_data_block(intnum,file_system *mf){
int count=0,data_list;
  data_list=mf->inode[num].direct_block;
if(data_list!=-1)
    count++;
  data_list=mf->inode[num].indirect_block;
if(data_list!=-1)
    count+=cal_indirect_num(mf->data[data_list].reg.storage);
return count;
}
void time_to_string(structtmct,char *ct_s){
  sprintf(ct_s,"%04d/%02d/%02d %02d:%02d:%02d",ct.tm_year+1900,ct.tm_mon+1,ct.tm_mday,ct.tm_hour,ct.tm_min,ct.tm_sec);
}
void myshowblock(char *order,file_system *mf){
int num;
  sscanf(order,"%*s %d",&num);
for(int i=0;i<128;i++)
    printf("%c",mf->data[num-1].reg.storage[i]);
}
void mytouch(char *order,file_system *mf){
time_t now_time;
  now_time=time(NULL);
int fin;
char *name,course[1000]="";
  sscanf(order,"%*s %s",course);
  fin=find_route(course,mf);
if(course[strlen(course)-1]=='/')
    course[strlen(course)-1]=0;
if(fin==-1){
int i_empty,d_empty;
    name=route_to_name(course);
    fin=find_route(course,mf);
if(name==NULL || name==(char *)1) ;
elseif(fin==-1){printf("오류 : regular 파일입니다.\n");return;}
    i_empty=find_empty_block(mf->sup_inode,64);
    mark_super(mf->sup_inode,i_empty);
mf->inode[i_empty].type='-';
mf->inode[i_empty].c_time=*(localtime(&now_time));
mf->inode[i_empty].size=0;
    d_empty=find_empty_block(mf->sup_data,128);
    mark_super(mf->sup_data,d_empty);
mf->inode[i_empty].direct_block=d_empty;
mf->inode[i_empty].indirect_block=-1;
mf->inode[i_empty].double_indirect_block=-1;
if(name==NULL)
      write_fname_finode(mf,course,i_empty,now);
elseif(name==(char *)1){
      write_fname_finode(mf,course+1,i_empty,0);
    }
else {
int ch;
      ch=find_route(course,mf);
      write_fname_finode(mf,name,i_empty,ch);
      free(name);
    }
  }
else{
mf->inode[fin].c_time=*(localtime(&now_time));
  }
}
void mymv(char *order,file_system *mf){
char temp[1000]="",f1course[1000]="",f2course[1000]="",*name2,*name1;;
int f1in,f2in,work_station,nwstation;
time_t now_time;
    now_time=time(NULL);
    sscanf(order,"%*s %s %s",f1course,f2course);
    f1in=find_route(f1course,mf);
if(f1in==-1){
      printf("오류 : 파일이없습니다.\n");
return ;
    }
    name1=route_to_name(f1course);
if(name1==NULL) {name1=f1course; nwstation=now;}
elseif(name1==(char *)1) {name1=f1course+1; nwstation=0;}
else {nwstation=find_route(f1course,mf);}
    f2in=find_route(f2course,mf);
if(f2in==-1){
      name2=route_to_name(f2course);
if(name2==NULL) {name2=f2course; work_station=now;}
elseif(name2==(char *)1) {name2=f2course+1; work_station=0;}
else {work_station=find_route(f2course,mf);}
if(work_station==nwstation){
        change_fname_finode(mf,name1,name2,work_station);
      }
else{
        remove_fname_finode(mf,name1,nwstation);
        write_fname_finode(mf,name2,f1in,work_station);
      }
mf->inode[f1in].c_time=*(localtime(&now_time));
    }
else {
if(f1in==f2in){
        printf("오류 : 같은파일입니다.\n");
return ;
      }
if(mf->inode[f2in].type=='d'){
if(f2in==nwstation){
          printf("오류 : 같은파일입니다.\n");
return ;
        }
else{
int *fin_temp,rn_temp,fch;
          fin_temp=list_data_block(f2in,mf);
          rn_temp=count_data_block(f2in,mf);
for(int i=0;i<rn_temp;i++){
            fch=find_file(mf->data[fin_temp[i]].dir,name1);
if(fch!=-1) break;
          }
if(fch!=-1){
            sprintf(temp,"myrm %s/%s",f2course,name1);
            myrm(temp,mf);
          }
          remove_fname_finode(mf,name1,nwstation);
          write_fname_finode(mf,name1,f1in,f2in);
          free(fin_temp);
        }
      }
else {
        sprintf(temp,"myrm %s",f2course);
        myrm(temp,mf);
        name2=route_to_name(f2course);
if(name2==NULL) {name2=f2course; work_station=now;}
elseif(name2==(char *)1) {name2=f2course+1; work_station=0;}
else {work_station=find_route(f2course,mf);}
if(work_station==nwstation){
          change_fname_finode(mf,name1,name2,work_station);
        }
else{
          remove_fname_finode(mf,name1,nwstation);
          write_fname_finode(mf,name2,f1in,work_station);
        }
      }
mf->inode[f1in].c_time=*(localtime(&now_time));
    }
}
void myrmdir(char *order,file_system *mf){
char course[1000]="",file_name[5]="",*name;
int temp,rn,*data_list,work_station;
  sscanf(order,"%*s %s",course);
  temp=find_route(course,mf);
if(temp==-1){
    printf("오류 : 파일이없습니다.\n");
return ;
  }
if(mf->inode[temp].type=='-'){
    printf("오류 : regular 파일입니다.\n");
return ;
  }
  data_list=list_data_block(temp,mf);
  rn=count_data_block(temp,mf);
if(mf->data[data_list[0]].dir.famount!=0){
    printf("오류 : %s is not empty\n",course);
return ;
  }
for(int i=0;i<rn;i++){
    reset_data_block(data_list[i],mf);
    unmark_super(mf->sup_data,data_list[i]);
  }
  reset_indirect(temp,mf);
  unmark_super(mf->sup_inode,temp);
  name=route_to_name(course);
if(name==NULL) {name=course; work_station=now;}
elseif(name==(char *)1) {name=course+1; work_station=0;}
else  {work_station=find_route(course,mf);}
  strncpy(file_name,name,4);
  remove_fname_finode(mf,file_name,work_station);
  free(data_list);
}
void myrm(char *order,file_system *mf){
char course[1000]="",file_name[5]="",*name;
int temp,rn,*data_list,work_station;
  sscanf(order,"%*s %s",course);
  temp=find_route(course,mf);
if(temp==-1){
    printf("오류 : 파일이없습니다.\n");
return ;
  }
if(mf->inode[temp].type=='d'){
    printf("오류 : directory 파일입니다.\n");
return ;
  }
  data_list=list_data_block(temp,mf);
  rn=count_data_block(temp,mf);
for(int i=0;i<rn;i++){
    reset_data_block(data_list[i],mf);
    unmark_super(mf->sup_data,data_list[i]);
  }
if(rn>1){
    reset_data_block(mf->inode[temp].indirect_block,mf);
    unmark_super(mf->sup_data,mf->inode[temp].indirect_block);
  }
  reset_indirect(temp,mf);
  unmark_super(mf->sup_inode,temp);
  name=route_to_name(course);
if(name==NULL) {name=course; work_station=now;}
elseif(name==(char *)1) {name=course+1; work_station=0;}
else  {work_station=find_route(course,mf);}
  strncpy(file_name,name,4);
  remove_fname_finode(mf,file_name,work_station);
  free(data_list);
}
void reset_data_block(intnum,file_system *mf){
for(int i=0;i<128;i++)
mf->data[num].reg.storage[i]=0;
}
int sort_str(constvoid *v1,constvoid *v2){
return strncmp(((ram_dfile *)(v1))->file_name,((ram_dfile *)(v2))->file_name,4);
}
void write_fname_finode(file_system *mf,char *myf_name,inti_empty,intfile_inode){
int temp,data_list,fc,fdc,present=0,parent=0;
  data_list=mf->inode[file_inode].direct_block;
  fc=mf->data[data_list].dir.famount++;
  present=mf->data[data_list].dir.present;
	 fdc=fc/20;
  if(fdc==0){
    strncpy(mf->data[data_list].dir.name_inode[fc].file_name,myf_name,4);
    mf->data[data_list].dir.name_inode[fc].file_inode=i_empty;
    if(fc==19){
      mf->inode[file_inode].indirect_block=find_empty_block(mf->sup_data,128);
      mark_super(mf->sup_data,mf->inode[file_inode].indirect_block);
      temp=mf->inode[file_inode].indirect_block;
      indirect_int_to_bit(mf->data[temp].reg.storage,find_empty_block(mf->sup_data,128));
      temp=find_empty_block(mf->sup_data,128);
      mark_super(mf->sup_data,temp);
      mf->data[temp].dir.present=present;
      mf->data[temp].dir.parent=parent;
    }
  }
  else if(fdc>=1&&fdc<=102){
    data_list=mf->inode[file_inode].indirect_block;
    data_list=indirect_bit_to_int(mf->data[data_list].reg.storage,fdc);
    strncpy(mf->data[data_list].dir.name_inode[fc%20].file_name,myf_name,4);
    mf->data[data_list].dir.name_inode[fc%20].file_inode=i_empty;
    if(fc%20==19&&fdc!=102){
      temp=mf->inode[file_inode].indirect_block;
      indirect_int_to_bit(mf->data[temp].reg.storage,find_empty_block(mf->sup_data,128));
      temp=find_empty_block(mf->sup_data,128);
      mark_super(mf->sup_data,temp);
      mf->data[temp].dir.present=present;
      mf->data[temp].dir.parent=parent;
    }
  }
}
void mypwd(char *order,file_system *mf){
  printf("%s\n",now_directory);
}
void mycd(char *order,file_system *mf,tree *root,tree **n2ow){
  char course[1000]="",(*nlist)[5],*temp,stack[1000][5]={""};
  int fin,i=1;
  tree *ws;
  sscanf(order,"%*s %s",course);
  if(course[strlen(course)-1]=='/')
    course[strlen(course)-1]=0;
  if(course[0]==0){
    now=0;
    for(int i=1;i<10000;i++)
      now_directory[i]=0;
    now_directory[0]='/';
    *n2ow=root;
  }
  else{
    fin=find_route(course,mf);
    if(fin==-1){
      printf("오류 : 파일이 없습니다.\n");
      return ;
    }
    if(mf->inode[fin].type=='-'){
      printf("오류 : regular 파일 입니다.\n");
      return ;
    }
    if (course[0]=='/') {
      for(int j=1;j<10000;j++)
        now_directory[j]=0;
        int count=1;
         while(1)
           {
              if(course[2]==0)
              {
                 count--;
                 break;
              }
              if(course[i]=='/' && course[i+1]!=0)
                 count++;
              else if(course[i]=='/'&&course[i+1]==0)
                 break;
              if(course[i] == 0)
                 break;
              i++;
           }
           nlist=(char (*)[5])calloc((count+1)*5,sizeof(char));
           nlist[0][0]='/';
           nlist[0][1]=0;
           nlist[0][2]=0;
           nlist[0][3]=0;
           if(count>=1){
             temp=strtok(course,"/");
             nlist[1][0]=temp[0];
             nlist[1][1]=temp[1];
             nlist[1][2]=temp[2];
             nlist[1][3]=temp[3];
            for(int i=2;i<=count;i++){
              temp=strtok(NULL,"/");
              nlist[i][0]=temp[0];
              nlist[i][1]=temp[1];
              nlist[i][2]=temp[2];
              nlist[i][3]=temp[3];
            }
          }
          ws=change_work_station(nlist,count+1,root,*n2ow);
          *n2ow=ws;
        i=0;
        while(1){
            strcpy(stack[i],ws->file_name);
            if(strncmp(stack[i],"/",1)==0)
              break;
            i++;
            ws=ws->parents;
        }
        for(int j=i-1;j>=0;j--){
          if(j==i-1)
            sprintf(now_directory,"/%s",stack[j]);
          else
            sprintf(now_directory+strlen(stack[j])+1,"/%s",stack[j]);
        }
    }
    else{
      for(int j=1;j<10000;j++)
        now_directory[j]=0;
        int count=0;
        if(course[strlen(course)-1]=='/')
         course[strlen(course)-1]=0;
        for(int k=0;k<=(int)strlen(course);k++){
          if(course[k]=='/'||course[k]==0){
            count++;
          }
        }
        nlist=(char (*)[5])calloc(count*5,sizeof(char));
          temp=strtok(course,"/");
          nlist[0][0]=temp[0];
          nlist[0][1]=temp[1];
          nlist[0][2]=temp[2];
          nlist[0][3]=temp[3];
         for(int k=1;k<count;k++){
           temp=strtok(NULL,"/");
           nlist[k][0]=temp[0];
           nlist[k][1]=temp[1];
           nlist[k][2]=temp[2];
           nlist[k][3]=temp[3];
         }
         ws=change_work_station(nlist,count,root,*n2ow);
        *n2ow=ws;
         i=0;
         while(1){
             strcpy(stack[i],ws->file_name);
             if(strncmp(stack[i],"/",1)==0)
               break;
             i++;
             ws=ws->parents;
         }
         for(int j=i-1;j>=0;j--){
           if(j==i-1)
             sprintf(now_directory,"/%s",stack[j]);
           else
             sprintf(now_directory+strlen(stack[j])+1,"/%s",stack[j]);
         }
    }
    now=fin;
  }

}
void indirect_int_to_bit(unsigned char *storage,short num){
  int temp,emp=103,bit_s;
  unsigned char first,second;
  for(int i=1;i<=102;i++){
    temp=indirect_bit_to_int(storage,i);
    if(temp==0){
      emp=i;
      break;
    }
  }
  emp=emp-1;
  bit_s=(emp*10)%8;
  temp=0;
  for(int i=0;i<8-bit_s;i++)
    temp+=pow(2,9-i);
  temp=num&temp;
  first=temp>>(2+bit_s);
  temp=0;
  for(int i=0;i<2+bit_s;i++)
    temp+=pow(2,1+bit_s-i);
  temp=num&temp;
  second=temp<<(6-bit_s);
  storage[emp]=storage[emp]|first;
  storage[emp+1]=storage[emp+1]|second;
}
void remove_indirect_bit(unsigned char *storage,short count,file_system *mf){
  int temp,emp=103,bit_s;
  unsigned char first=0,second=0;
  for(int j=0;j<count;j++){
    first=0;
    for(int i=1;i<=102;i++){
      temp=indirect_bit_to_int(storage,i);
      if(temp==0){
        emp=i;
        break;
      }
    }
    temp=indirect_bit_to_int(storage,emp-1);
    reset_data_block(temp,mf);
    unmark_super(mf->sup_data,temp);
    emp=emp-2;
    bit_s=(emp*10)%8;
    for(int i=0;i<bit_s;i++)
      first+=pow(2,7-i);
    storage[emp]=storage[emp]&first;
    storage[emp+1]=storage[emp+1]&second;
  }
}
int *list_empty_db(unsigned char *check,int size_c){
	int *db_empty;
	db_empty=(int *)malloc(size_c*sizeof(int));
	for(int i=0;i<size_c;i++){
		db_empty[i]=find_empty_block(check,128);
		mark_super(check,db_empty[i]);
	}
	return db_empty;
}
int cal_block(int file_size){
	int size_c;
  if(file_size%128==0&&file_size!=0)
    file_size-=1;
	size_c=file_size/128+1;
	if(size_c==1);
	else if(size_c>=2&&size_c<=103) size_c+=1;
	else if(size_c>=104&&size_c<=1012) size_c+=2+(size_c-2)/102;
	return size_c;
}
tree *change_work_station(char (*name)[5],int num,tree *root,tree *now){
  tree *temp;
  if(strncmp(name[0],"/",1)==0){
    if(num==1){
      return root;
    }
    temp=root->children;
    for(int i=1;i<num;i++){
      if(i!=1)
        temp=temp->children;
      while(1){
        if(strncmp("..",name[i],2)==0){
          temp=temp->parents->parents;
          break;
        }
        else if(strncmp(".",name[i],1)==0){
          temp=temp->parents;
          break;
        }
        else if(strncmp(temp->file_name,name[i],4)==0){
          break;
        }
        else
          temp=temp->siblings;
      }
    }
    return temp;
  }
  else {
    temp=now->children;
    for(int i=0;i<num;i++){
      if(i!=0)
        temp=temp->children;
      while(1){
        if(strncmp("..",name[i],2)==0){
          temp=temp->parents->parents;
          break;
        }
        else if(strncmp(".",name[i],1)==0){
          temp=temp->parents;
          break;
        }
        else if(strncmp(temp->file_name,name[i],4)==0)
          break;
        else
          temp=temp->siblings;
      }
    }
    return temp;
  }
}
void recursion_print(int deep,tree *work_station){
  while(1){
    for(int i=1;i<deep*3;i++)
      printf("-");
    printf("* %s\n",work_station->file_name);
    if(work_station->children!=NULL)
      recursion_print(deep+1,work_station->children);
    if(work_station->siblings==NULL)
      return;
    work_station=work_station->siblings;
  }
}
int count_empty_block(unsigned char *check,int length){
  int count=0;
  unsigned char bit_ch,bit_temp;
  for(int i=0;i<length;i++){
		for(int j=0;j<8;j++){
			bit_ch=pow(2,7-j);
			bit_temp=check[i]&bit_ch;
			if(bit_temp==0){
				count++;
			}
		}
	}
  return count;
}
void mymkdir(char *order,file_system *mf){
  char course[1000]="",dname[5]="",temp[1000]="";
  int fin,inum,data_list;
  time_t now_time;
  now_time=time(NULL);
  sscanf(order,"%*s %s",course);
  if(course[strlen(course)-1]=='/')
   course[strlen(course)-1]=0;
  if(course[0]=='/'){
   int count=1,i=1;
     while(1)
     {
        if(course[2]==0)
        {
           count--;
           break;
        }
        if(course[i]=='/' && course[i+1]!=0)
           count++;
        else if(course[i]=='/'&&course[i+1]==0)
           break;
        if(course[i] == 0)
           break;
        i++;
     }
     if(count!=1){
      for(int i=strlen(course)-1,j=0;;i--,j++){
        if(course[i]=='/')
          break;
        else{
          temp[j]=course[i];
          course[i]=0;
        }
      }
      for(int i=0;i<4;i++)
        dname[i]=temp[strlen(temp)-1-i];
      fin=find_route(course,mf);
      }
      else{
         strncpy(dname,course+1,4);
         fin=0;
      }
      if(fin==-1){
        printf("오류 : 파일이 없습니다.\n");
        return ;
      }
      else{
        int *data_list,rn,temp;
        data_list=list_data_block(fin,mf);
        rn=count_data_block(fin,mf);
        for(int i=0;i<rn;i++){
          temp=find_file(mf->data[data_list[i]].dir,dname);
          if(temp!=-1) {
            printf("오류 : 파일이 이미 있습니다.\n");
            return ;
          }
        }
        free(data_list);
      }
      inum=find_empty_block(mf->sup_inode,64);
      mark_super(mf->sup_inode,inum);
      data_list=find_empty_block(mf->sup_data,128);
      mark_super(mf->sup_data,data_list);
      mf->inode[inum].type='d';
      mf->inode[inum].c_time=*(localtime(&now_time));
      mf->inode[inum].size=0;
      mf->inode[inum].direct_block=data_list;
      mf->inode[inum].indirect_block=-1;
      mf->inode[inum].double_indirect_block=-1;
      mf->data[data_list].dir.present=inum;
      mf->data[data_list].dir.parent=fin;
      mf->data[data_list].dir.famount=0;
      write_fname_finode(mf,dname,inum,fin);
  }
  else{
    int count=0;
    if(course[strlen(course)-1]=='/')
     course[strlen(course)-1]=0;
    for(int i=0;i<=(int)strlen(course);i++){
      if(course[i]=='/'||course[i]==0){
        count++;
      }
    }
    if(count!=1){
     for(int i=strlen(course)-1,j=0;;i--,j++){
       if(course[i]=='/')
         break;
       else{
         temp[j]=course[i];
         course[i]=0;
       }
     }
     for(int i=0;i<4;i++)
       dname[i]=temp[strlen(temp)-1-i];
     fin=find_route(course,mf);
    }
    else {
       strncpy(dname,course,4);
       fin=now;
    }
    if(fin==-1){
      printf("오류 : 파일이 없습니다.\n");
      return ;
    }
    else{
      int *data_list,rn,temp;
      data_list=list_data_block(fin,mf);
      rn=count_data_block(fin,mf);
      for(int i=0;i<rn;i++){
        temp=find_file(mf->data[data_list[i]].dir,dname);
        if(temp!=-1) {
          printf("오류 : 파일이 이미 있습니다.\n");
          return ;
        }
      }
      free(data_list);
    }
    inum=find_empty_block(mf->sup_inode,64);
    mark_super(mf->sup_inode,inum);
    data_list=find_empty_block(mf->sup_data,128);
    mark_super(mf->sup_data,data_list);
    mf->inode[inum].type='d';
    mf->inode[inum].c_time=*(localtime(&now_time));
    mf->inode[inum].size=0;
    mf->inode[inum].direct_block=data_list;
    mf->inode[inum].indirect_block=-1;
    mf->inode[inum].double_indirect_block=-1;
    mf->data[data_list].dir.present=inum;
    mf->data[data_list].dir.parent=fin;
    mf->data[data_list].dir.famount=0;
    write_fname_finode(mf,dname,inum,fin);
  }
}
void myshowfile(char *order,file_system *mf){
  int num1,num2,*data_list,fin,size,n1b,n2b,n1r,n2r;
  char course[1000];
  sscanf(order,"%*s %d %d %s",&num1,&num2,course);
  fin=find_route(course,mf);
  if(fin==-1){
      printf("오류 : 파일이 없습니다.\n");
      return ;
  }
  size=mf->inode[fin].size;
  if(num1>size||num2>size){
    printf("오류 : 파일 크기 %d\n",size);
    return;
  }
  data_list=list_data_block(fin,mf);
  if(num1%128==0&&num1!=0)
    n1b=num1/128-1;
  else
    n1b=num1/128;
  if(num2%128==0&&num2!=0)
    n2b=num2/128-1;
  else
    n2b=num2/128;
  n1r=num1%128;
  if(n1r==0&&num1!=0)
	     n1r=128;
  n2r=num2%128;
  if(n2r==0&&num2!=0)
      n2r=128;
  if(n1b==n2b){
    for(int i=n1r-1;i<n2r;i++)
    printf("%c",mf->data[data_list[n1b]].reg.storage[i]);
    return ;
  }
  for(int i=n1r-1;i<128;i++){
    printf("%c",mf->data[data_list[n1b]].reg.storage[i]);
  }
  for(int i=n1b+1;i<=n2b-1;i++){
    for(int j=0;j<128;j++)
      printf("%c",mf->data[data_list[i]].reg.storage[j]);
  }
  for(int i=0;i<n2r;i++){
    printf("%c",mf->data[data_list[n2b]].reg.storage[i]);
  }
}
void mystate(char *order,file_system *mf){
  int data_count;
  data_count=count_empty_block(mf->sup_inode,64);
  printf("free inode : %d\n",data_count);
  data_count=count_empty_block(mf->sup_data,128);
  printf("free data block : %d\n",data_count);
}
void mycpfrom(char *order,file_system *mf){
  char myf_name[5]="";
	char f_name[1000];
  int file_size,i_empty,*db_empty,size_c,data_count,work_station=now;
	FILE *fop;
	time_t ct;
	ct=time(NULL);
  sscanf(order,"%*s %s %c %c %c %c ",f_name,&myf_name[0],&myf_name[1],&myf_name[2],&myf_name[3]);
  fop=fopen(f_name,"rb");
  if(fop==NULL){
    printf("오류 : 파일이 없습니다.\n");
    return ;
  }
  {
    int *data_list,rn,temp;
    data_list=list_data_block(work_station,mf);
    rn=count_data_block(work_station,mf);
    for(int i=0;i<rn;i++){
      temp=find_file(mf->data[data_list[i]].dir,myf_name);
      if(temp!=-1) {
        printf("오류 : 파일이 이미 있습니다.\n");
        return ;
      }
    }
    free(data_list);
  }
  fseek(fop,0,SEEK_END);
	file_size=ftell(fop);
	fseek(fop,0,SEEK_SET);
  size_c=cal_block(file_size);
  data_count=count_empty_block(mf->sup_data,128);

  if(size_c<=data_count){
		i_empty=find_empty_block(mf->sup_inode,64);
		mark_super(mf->sup_inode,i_empty);

		mf->inode[i_empty].type='-';
		mf->inode[i_empty].c_time=*(localtime(&ct));
		mf->inode[i_empty].size=file_size;
		db_empty=list_empty_db(mf->sup_data,size_c);
		write_fname_finode(mf,myf_name,i_empty,work_station);

		if(size_c==1){
			mf->inode[i_empty].direct_block=db_empty[0];
			mf->inode[i_empty].indirect_block=-1;
			mf->inode[i_empty].double_indirect_block=-1;
			fread(mf->data[db_empty[0]].reg.storage,file_size,1,fop);
		}
		else if(size_c>=3&&size_c<=104){
			mf->inode[i_empty].direct_block=db_empty[0];
			mf->inode[i_empty].indirect_block=db_empty[size_c-1];
			mf->inode[i_empty].double_indirect_block=-1;
      for(int i=1;i<size_c-1;i++)
        indirect_int_to_bit(mf->data[db_empty[size_c-1]].reg.storage,db_empty[i]);
      for(int i=0;i<size_c-2;i++)
        fread(mf->data[db_empty[i]].reg.storage,128,1,fop);
      {
        int temp;
        temp=file_size%128;
        if(file_size/128!=0&&temp==0)
          temp=128;
        fread(mf->data[db_empty[size_c-2]].reg.storage,temp,1,fop);
      }
    }
	}
	else
		printf("오류 : 공간 부족\n");
  free(db_empty);
  fclose(fop);
}
int find_empty_block(unsigned char *check,int length){
	unsigned char bit_ch,bit_temp;
	for(int i=0;i<length;i++){
		for(int j=0;j<8;j++){
			bit_ch=pow(2,7-j);
			bit_temp=check[i]&bit_ch;
			if(bit_temp==0){
				return (i*8+j);
			}
		}
	}
	return -1;
}
tree * create_tree(char *file_name,int file_inode){
  tree *temp;
  temp=(tree *)calloc(1,sizeof(tree));
  strncpy(temp->file_name,file_name,4);
  temp->file_inode=file_inode;
  temp->parents=NULL;
  temp->siblings=NULL;
  temp->children=NULL;
  return temp;
}
void mark_super(unsigned char *check,int num){
	int room,square,bit;
	room=num/8;
	square=num%8;
	bit=pow(2,7-square);
	check[room]=check[room]|bit;
}
void unmark_super(unsigned char *check,int num){
	int room,square,bit;
	room=num/8;
	square=num%8;
	bit=pow(2,7-square);
	bit=255-bit;
	check[room]=check[room]&bit;
}
void mycpto(char *order,file_system *mf){
  char myf_name[5]="",cpf_name[1000]="";
  int fin,rn,*data_list,size;
  FILE *fop;
  sscanf(order,"%*s %4s",myf_name);
  sscanf(order,"%*s %*s %s",cpf_name);
  data_list=list_data_block(now,mf);
  rn=count_data_block(now,mf);
  for(int i=0;i<rn;i++){
    fin=find_file(mf->data[data_list[i]].dir,myf_name);
    if(fin!=-1) break;
  }
  if(fin==-1){
    printf("오류 : 파일이 없습니다.\n");
    return ;
  }
  free(data_list);
  data_list=list_data_block(fin,mf);
  rn=count_data_block(fin,mf);
  size=mf->inode[fin].size;
  fop=fopen(cpf_name,"wb");
  for(int i=0;i<rn;i++){
    if(i==rn-1){
      fwrite(mf->data[data_list[i]].reg.storage,size,1,fop);
    }
    else{
      size=size-128;
      fwrite(mf->data[data_list[i]].reg.storage,128,1,fop);
    }
  }
  fclose(fop);
}
void myls(char *order,file_system *mf){
	char option[1000]="",course[1000]="",ct_s[20];
	short dfn,work_station;
  int data_list;
  ram_dfile *srt;
	sscanf(order,"%*s %s %s",option,course);
  if(option[0]=='-'){
    if(course[0]==0)
      work_station=now;
    else
      work_station=find_route(course,mf);
  }
  else if(option[0]==0){
    work_station=now;
  }
  else
    work_station=find_route(option,mf);
  if(mf->inode[work_station].type=='-'){
    printf("오류 : directory 파일 입니다.\n");
    return ;
  }
  dfn=mf->data[mf->inode[work_station].direct_block].dir.famount;
  srt=(ram_dfile *)calloc(dfn,sizeof(ram_dfile));
  if(dfn<=20){
    for(int i=0;i<dfn;i++){
       strncpy(srt[i].file_name,mf->data[mf->inode[work_station].direct_block].dir.name_inode[i].file_name,4);
       srt[i].file_name[4]=0;
       srt[i].file_inode=mf->data[mf->inode[work_station].direct_block].dir.name_inode[i].file_inode;
    }
  }
  else if(dfn>20){
    for(int i=0;i<20;i++){
      strncpy(srt[i].file_name,mf->data[mf->inode[work_station].direct_block].dir.name_inode[i].file_name,4);
      srt[i].file_name[4]=0;
      srt[i].file_inode=mf->data[mf->inode[work_station].direct_block].dir.name_inode[i].file_inode;
    }
    for(int i=0;i<dfn/20-1;i++){
      data_list=indirect_bit_to_int(mf->data[mf->inode[work_station].indirect_block].reg.storage,i+1);
      for(int j=0;j<20;j++){
        strncpy(srt[(i+1)*20+j].file_name,mf->data[mf->inode[data_list].direct_block].dir.name_inode[j].file_name,4);
        srt[(i+1)*20+j].file_name[4]=0;
        srt[(i+1)*20+j].file_inode=mf->data[mf->inode[data_list].direct_block].dir.name_inode[j].file_inode;
      }
    }
    data_list=indirect_bit_to_int(mf->data[mf->inode[work_station].indirect_block].reg.storage,dfn/20);
    {
      int temp;
      temp=dfn%20;
      if(dfn/20!=0&&temp==0)
        temp=20;
      for(int i=0;i<temp;i++){
        strncpy(srt[(dfn/20)*20+i].file_name,mf->data[data_list].dir.name_inode[i].file_name,4);
        srt[(dfn/20)*20+i].file_name[4]=0;
        srt[(dfn/20)*20+i].file_inode=mf->data[data_list].dir.name_inode[i].file_inode;
      }
    }
  }
  qsort(srt,dfn,sizeof(ram_dfile),sort_str);
  if(option[0]==0){
    printf(".\n..\n");
    for(int i=0;i<dfn;i++)
      printf("%s\n",srt[i].file_name);
  }
  else if(option[0]=='-'&&option[1]=='l'&&option[2]==0){
    int fin;
    fin=mf->data[mf->inode[work_station].direct_block].dir.present;
    time_to_string(mf->inode[fin].c_time,ct_s);
    printf("d %6d %s .\n",0,ct_s);
    fin=mf->data[mf->inode[work_station].direct_block].dir.parent;
    time_to_string(mf->inode[fin].c_time,ct_s);
    printf("d %6d %s ..\n",0,ct_s);
    for(int i=0;i<dfn;i++){
      fin=srt[i].file_inode;
      time_to_string(mf->inode[fin].c_time,ct_s);
      printf("%c %6d %s %s\n",mf->inode[fin].type,mf->inode[fin].size,ct_s,srt[i].file_name);
    }
  }
  else if(option[0]=='-'&&option[1]=='i'&&option[2]==0){
    int fin;
    fin=mf->data[mf->inode[work_station].direct_block].dir.present;
    printf("%3d .\n",fin+1);
    fin=mf->data[mf->inode[work_station].direct_block].dir.parent;
    printf("%3d ..\n",fin+1);
    for(int i=0;i<dfn;i++){
      fin=srt[i].file_inode;
      printf("%3d %s\n",fin+1,srt[i].file_name);
    }
  }
  else if((option[0]=='-'&&option[1]=='l'&&option[2]=='i')||(option[0]=='-'&&option[1]=='i'&&option[2]=='l')){
    int fin;
    fin=mf->data[mf->inode[work_station].direct_block].dir.present;
    time_to_string(mf->inode[fin].c_time,ct_s);
    printf("%3d d %6d %s .\n",fin+1,0,ct_s);
    fin=mf->data[mf->inode[work_station].direct_block].dir.parent;
    time_to_string(mf->inode[fin].c_time,ct_s);
    printf("%3d d %6d %s ..\n",fin+1,0,ct_s);
    for(int i=0;i<dfn;i++){
      fin=srt[i].file_inode;
      time_to_string(mf->inode[fin].c_time,ct_s);
      printf("%3d %c %6d %s %s\n",fin+1,mf->inode[fin].type,mf->inode[fin].size,ct_s,srt[i].file_name);
    }
  }
  else {
    printf(".\n..\n");
    for(int i=0;i<dfn;i++)
      printf("%s\n",srt[i].file_name);
  }
  free(srt);
}
void recursion_tree(file_system *mf,tree *work_station){
  int famount,*data_list;
  tree *temp;
  if(mf->inode[work_station->file_inode].type=='-')
    return;
  famount=mf->data[mf->inode[work_station->file_inode].direct_block].dir.famount;
  if(famount==0)
    return;
  data_list=list_data_block(work_station->file_inode,mf);
  work_station->children=create_tree(mf->data[data_list[0]].dir.name_inode[0].file_name,mf->data[data_list[0]].dir.name_inode[0].file_inode);
  work_station->children->parents=work_station;
  temp=work_station;
  work_station=work_station->children;
  recursion_tree(mf,work_station);
  for(int i=1;i<famount;i++){
    work_station->siblings=create_tree(mf->data[data_list[i/20]].dir.name_inode[i%20].file_name,mf->data[data_list[i/20]].dir.name_inode[i%20].file_inode);
    work_station->siblings->parents=temp;
    work_station=work_station->siblings;
    recursion_tree(mf,work_station);
  }
  free(data_list);
}
int find_file(directory storage,char *name){
  int fin=-1,temp;
  for(int i=0;i<20;i++){
    temp=strncmp(storage.name_inode[i].file_name,name,4);
    if(temp==0){
      fin=storage.name_inode[i].file_inode;
      break;
    }
  }
  return fin;
}
void remove_fname_finode(file_system *mf,char *myf_name,int work_station){
  int *data_list,rn,temp,fin,temp2,last=19,last_rn;
  data_list=list_data_block(work_station,mf);
  rn=count_data_block(work_station,mf);
  for(int i=0;i<rn;i++){
    fin=find_file(mf->data[data_list[i]].dir,myf_name);
    if(fin!=-1) {temp=i; break;}
  }
  if(fin==-1){
    printf("오류 : 파일이 없습니다.\n");
    free(data_list);
    return ;
  }
  mf->data[data_list[0]].dir.famount-=1;
  for(int i=0;i<20;i++){
    if(mf->data[data_list[rn-1]].dir.name_inode[i].file_inode==0){
        last=i-1;
        last_rn=rn-1;
        break;
    }
  }
  if(last==-1){
    last=19;
    last_rn=rn-2;
    remove_indirect_bit(mf->data[mf->inode[work_station].indirect_block].reg.storage,1,mf);
    if(mf->data[data_list[0]].dir.famount<20){
      unmark_super(mf->sup_data,mf->inode[work_station].indirect_block);
      reset_data_block(mf->inode[work_station].indirect_block,mf);
      mf->inode[work_station].indirect_block=-1;
    }
  }
  for(int i=0;i<20;i++){
    if(strncmp(mf->data[data_list[temp]].dir.name_inode[i].file_name,myf_name,4)==0){
        temp2=i;
        break;
    }
  }
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[0]=mf->data[data_list[last_rn]].dir.name_inode[last].file_name[0];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[1]=mf->data[data_list[last_rn]].dir.name_inode[last].file_name[1];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[2]=mf->data[data_list[last_rn]].dir.name_inode[last].file_name[2];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[3]=mf->data[data_list[last_rn]].dir.name_inode[last].file_name[3];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_inode=mf->data[data_list[last_rn]].dir.name_inode[last].file_inode;
  mf->data[data_list[last_rn]].dir.name_inode[last].file_name[0]=0;
  mf->data[data_list[last_rn]].dir.name_inode[last].file_name[1]=0;
  mf->data[data_list[last_rn]].dir.name_inode[last].file_name[2]=0;
  mf->data[data_list[last_rn]].dir.name_inode[last].file_name[3]=0;
  mf->data[data_list[last_rn]].dir.name_inode[last].file_inode=0;
  free(data_list);
}
void change_fname_finode(file_system *mf,char *myf_name,char *change_name,int work_station){
  int *data_list,rn,temp,fin,temp2;
  data_list=list_data_block(work_station,mf);
  rn=count_data_block(work_station,mf);
  for(int i=0;i<rn;i++){
    fin=find_file(mf->data[data_list[i]].dir,myf_name);
    if(fin!=-1) {temp=i; break;}
  }
  if(fin==-1){
    printf("오류 : 파일이 없습니다.\n");
    free(data_list);
    return ;
  }
  for(int i=0;i<20;i++){
    if(strncmp(mf->data[data_list[temp]].dir.name_inode[i].file_name,myf_name,4)==0){
        temp2=i;
        break;
    }
  }
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[0]=change_name[0];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[1]=change_name[1];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[2]=change_name[2];
  mf->data[data_list[temp]].dir.name_inode[temp2].file_name[3]=change_name[3];
  free(data_list);
}
int cal_indirect_num(unsigned char *storage){
  int temp,count=0;
  for(int i=1;i<=102;i++){
    temp=indirect_bit_to_int(storage,i);
    if(temp==0){
      count=i-1;
      break;
    }
  }
  return count;
}
void print_tree(char *order,tree *root,tree *now,file_system *mf){
  char course[1000]="",bck[1000],(*nlist)[5],*temp;
  int i=1,deep=0;
  tree *work_station=NULL;
  sscanf(order,"%*s %s",course);
  strcpy(bck,course);
  if(course[0]==0){
    printf("%s\n",now->file_name);
    if(mf->data[mf->inode[now->file_inode].direct_block].dir.famount==0)
      return ;
    deep=1;
    work_station=now->children;
    recursion_print(deep,work_station);
  }
  else if(course[0]=='/'){
    if(find_route(course,mf)==-1){
      printf("오류 : 파일이 없습니다.\n");
      return ;
    }
    if(mf->inode[find_route(course,mf)].type=='-'){
      printf("오류 : regular 파일 입니다.\n");
      return ;
    }
    int count=1;
     while(1)
       {
          if(course[2]==0)
          {
             count--;
             break;
          }
          if(course[i]=='/' && course[i+1]!=0)
             count++;
          else if(course[i]=='/'&&course[i+1]==0)
             break;
          if(course[i] == 0)
             break;
          i++;
       }
       nlist=(char (*)[5])calloc((count+1)*5,sizeof(char));
       nlist[0][0]='/';
       nlist[0][1]=0;
       nlist[0][2]=0;
       nlist[0][3]=0;
       if(count>=1){
         temp=strtok(bck,"/");
         nlist[1][0]=temp[0];
         nlist[1][1]=temp[1];
         nlist[1][2]=temp[2];
         nlist[1][3]=temp[3];
        for(int i=2;i<=count;i++){
          temp=strtok(NULL,"/");
          nlist[i][0]=temp[0];
          nlist[i][1]=temp[1];
          nlist[i][2]=temp[2];
          nlist[i][3]=temp[3];
        }
      }
      work_station=change_work_station(nlist,count+1,root,work_station);
      printf("%s\n",work_station->file_name);
      if(work_station->children==NULL)
        return ;
      deep=1;
      work_station=work_station->children;
      recursion_print(deep,work_station);

  }
  else{
    if(find_route(course,mf)==-1){
      printf("오류 : 파일이 없습니다.\n");
      return ;
    }
    if(mf->inode[find_route(course,mf)].type=='-'){
      printf("오류 : regular 파일 입니다.\n");
      return ;
    }
    int count=0;
    if(course[strlen(course)-1]=='/')
     course[strlen(course)-1]=0;
    for(int i=0;i<=(int)strlen(course);i++){
      if(course[i]=='/'||course[i]==0){
        count++;
      }
    }
    nlist=(char (*)[5])calloc(count*5,sizeof(char));
      temp=strtok(bck,"/");
      nlist[0][0]=temp[0];
      nlist[0][1]=temp[1];
      nlist[0][2]=temp[2];
      nlist[0][3]=temp[3];
     for(int i=1;i<count;i++){
       temp=strtok(NULL,"/");
       nlist[i][0]=temp[0];
       nlist[i][1]=temp[1];
       nlist[i][2]=temp[2];
       nlist[i][3]=temp[3];
     }
     work_station=change_work_station(nlist,count,root,now);
     printf("%s\n",work_station->file_name);
     if(work_station->children==NULL)
       return ;
     deep=1;
     work_station=work_station->children;
     recursion_print(deep,work_station);
  }
}
int indirect_bit_to_int(unsigned char *storage,int num){
  int ind,bit_s,con=0,stack[10],temp;
  ind=num+(num-1)/4;
  bit_s=((num-1)*10)%8;
  for(int i=0;i<8-bit_s;i++){
    temp=pow(2,7-bit_s-i);
    temp=storage[ind-1]&temp;
    if(temp!=0)
      stack[i]=1;
    else
      stack[i]=0;
  }
  for(int i=8-bit_s;i<10;i++){
    temp=pow(2,7-i+(8-bit_s));
    temp=storage[ind]&temp;
    if(temp!=0)
      stack[i]=1;
    else
      stack[i]=0;
  }
  for(int i=0;i<10;i++)
    con+=stack[i]*pow(2,9-i);
  return con;
}
