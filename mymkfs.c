#include<stdio.h>
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
int main(){
  FILE *fop;
  time_t now;
  now=time(NULL);
  fop=fopen("myfs","r");
	if(fop!=NULL){
		printf("오류 : 파일이 이미 있습니다.\n");
		return 0;
	}
	else
		fop=fopen("myfs","wb");
  file_system mf={""};
  mf.sup_inode[0]=128;
  mf.sup_data[0]=128;
  mf.inode[0].type='d';
  mf.inode[0].c_time=*(localtime(&now));
  mf.inode[0].size=0;
  mf.inode[0].direct_block=0;
  mf.inode[0].indirect_block=-1;
  mf.inode[0].double_indirect_block=-1;
  mf.data[0].dir.present=0;
  mf.data[0].dir.parent=0;
  mf.data[0].dir.famount=0;
  fwrite(&mf,sizeof(mf),1,fop);
  fclose(fop);
  return 0;
}
