#define _CRT_SECURE_NO_WARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define START 0
#define CURRENT 1
#define END 2

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

typedef struct superblock
{
	int TotalInodes;
	int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int ActualFileSize;
	int FileType;
	char*Buffer;
	int LinkCount;
	int ReferenceCount;
	int Permission;
	struct inode*next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void DisplayHelp()
{
}

int GetFDFromName(char*name)
{
	int i = 0;
	if(SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		return -1;
	}
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable != NULL)
		{
			if((strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)) == 0)
			{
				break;
			}
		}
		i++;
	}
	if(i == 50)
	{
		return -1;
	}
	else
	{
		return i;
	}
}

PINODE Get_Inode(char*name)
{
	PINODE temp = head;

	if((name == NULL)||(SUPERBLOCKobj.FreeInodes == MAXINODE))
	{
		return NULL;
	}
	while(temp != NULL)
	{
		if(strcmp(name,temp->FileName) == 0)
		{
			break;
		}
		temp = temp->next;
	}
	return temp;
}

void CreateDILB()
{
	PINODE newn = NULL;
	PINODE temp = head;
	int i = 1;
	
	while(i <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));
		newn->LinkCount = newn->ReferenceCount = 0;
		newn->FileType = newn->FileSize = 0;
		newn->Buffer = NULL;
	
		newn->InodeNumber = i;

		if(temp == NULL)
		{
			head = newn;
			temp = head;
		}
		else
		{
			temp->next = newn;
			temp = temp->next;
		}
		i++;
	}
}

void InitializeSuperBlock()
{
	int i = 0;
	
	while(i < 50)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}
	(SUPERBLOCKobj.TotalInodes) = MAXINODE;
	(SUPERBLOCKobj.FreeInodes) = MAXINODE;
}

void ls_File()
{
	PINODE temp = head;

	if(SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		printf("Error : There are no file:\n");
		return;
	}

	printf("\nFile Name\tInodeNumber\tFileSize\tLinkCount\n");
	printf("\n--------------------------------------------------------------------\n");
	
	while(temp != NULL)
	{
		if(temp->FileType != 0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->ActualFileSize,temp->LinkCount);
		}
		temp = temp->next;
	}
	printf("\n--------------------------------------------------------------------\n");
}

void CloseAllFiles()
{
	int i = 0;
	if(SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		printf("ERROR : There is no Files\n");
		return;
	}
	
	while(i < 50)
	{
		if(UFDTArr[i].ptrfiletable != NULL)
		{
			UFDTArr[i].ptrfiletable->readoffset = 0;
			UFDTArr[i].ptrfiletable->writeoffset = 0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
		}
		i++;
	}
}

int StatFile(char*name)
{
	PINODE temp = head;
	if(name == NULL)
	{
		return -1;
	}
	
	if(SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		return -2;
	}
	while(temp != NULL)
	{
		if(strcmp(name,(temp->FileName)) == 0)
		{
			break;
		}
		temp = (temp->next);
	}
	if(temp == NULL)
	{
		return -2;
	}
	
	printf("\n------------------ Statical Information about the file -----------------\n");
	printf("FileName : %s\n",temp->FileName);
	printf("Inode Number : %d\n",temp->InodeNumber);
	printf("File Size : %d\n",temp->FileSize);
	printf("Actual File Size : %d\n",temp->ActualFileSize);
	printf("Link Count : %d\n",temp->LinkCount);
	printf("Reference Count : %d\n",temp->ReferenceCount);
	
	if(temp->Permission == 1)
	{
		printf("File Permission : Read Only\n");
	}
	else if(temp->Permission == 2)
	{
		printf("File Permission : Write Only\n");
	}
	else if(temp->Permission == 3)
	{
		printf("File Permission : Read And Write\n");
	}

	printf("---------------------------------------------------------------------------------------\n");
	return 0;
}

int fstat_file(int fd)
{
	PINODE temp = head;
	
	if((fd < 0)&&(fd > (MAXINODE - 1)))
	{
		return -1;
	}
	if(UFDTArr[fd].ptrfiletable == NULL)
	{
		return -2;
	}
	temp = UFDTArr[fd].ptrfiletable->ptrinode;

	printf("\n---------------------------------Statistical Information about the file-------------------------\n");
	printf("File Name : %s\n",temp->FileName);
	printf("Inode Number : %d\n",temp->InodeNumber);
	printf("File Size : %d\n",temp->FileSize);
	printf("Actual File Size : %d\n",temp->ActualFileSize);
	printf("Link Count : %d\n",temp->LinkCount);
	printf("Reference Count : %d\n",temp->ReferenceCount);
	
	if(temp->Permission == 1)
	{
		printf("File Permission : Read Only\n");
	}
	else if(temp->Permission == 2)
	{
		printf("File Permission : Write Only\n");
	}
	else if(temp->Permission == 3)
	{
		printf("File Permission : Read And Write\n");
	}
	printf("\n-----------------------------------------------------------------------------------------------------------\n");

	return 0;
}

int CloseFileByName(char*name)
{
	int i = 0;
	
	i = GetFDFromName(name);
	
	if(i == -1)
	{
		return -1;
	}

	UFDTArr[i].ptrfiletable->readoffset = 0;
	UFDTArr[i].ptrfiletable->writeoffset = 0;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

	return 0;
}

int rm_File()
{
	int fd = 0;
	
	if(fd == -1)
	{
		return -1;
	}
	
	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	if((UFDTArr[fd].ptrfiletable->ptrinode->LinkCount) == 0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
		free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
	}
	
	UFDTArr[fd].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInodes)++;

	return 0;
}

void Man(char*name)
{
	if(name == NULL)
	{
		return;
	}
	
	if(strcmp(name,"create") == 0)
	{
		printf("Description : Use to create new regular file\n");
		printf("Usage : create file_name permission\n");
	}
	else if(strcmp(name,"read") == 0)
	{
		printf("Description : Used to read the data from file\n");
		printf("Usage : read file_name No_of_Bytes_To_Read\n");
	}
	else if(strcmp(name,"write") == 0)
	{
		printf("Description : Use to write the data into the file\n");
		printf("Usage : write file_name\n");
	}
	else if(strcmp(name,"ls") == 0)
	{
		printf("Description : Used to list all information of files\n");
		printf("Usage : ls\n");
	}
	else if(strcmp(name,"stat") == 0)
	{
		printf("Description : USed to display information of file\n");
		printf("Usage : stat File_Name\n");
	}
	else if(strcmp(name,"fstat") == 0)
	{
		printf("Description : Used to dislay information of file\n");
		printf("Usage : fstat File_Descriptor\n");
	}
	else if(strcmp(name,"trucate") == 0)
	{
		printf("Description : remove the data\n");
		printf("Usage : truncate File_Name\n");
	}
	else if(strcmp(name,"open") == 0)
	{
		printf("Description : Used to open existing file\n");
		printf("Usage : open file_name mode\n");
	}
	else if(strcmp(name,"close") == 0)
	{
		printf("Description : Used to close the file\n");
		printf("Usage : close File_Name\n");
	}
	else if((strcmp(name,"closeall")) == 0)
	{
		printf("Description : Used to close all the opened file\n");
		printf("Usage : closeall\n");
	}
	else if((strcmp(name,"lseek")) == 0)
	{
		printf("Description : Used to change file offset\n");
		printf("Usage : lseek file_Name Change_In_Offset Start_point\n");
	}
	else if((strcmp(name,"rm")) == 0)
	{
		printf("Description : Used to remove the file\n");
		printf("Usage : rm File_Name\n");
	}	
	else
	{
		printf("ERROR : No manual Entry available\n");
	}
}

int WriteFile(int fd, char*arr,int iSize)
{
	if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != WRITE + READ))
	{
		return -1;
	}
	if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != READ+WRITE))
	{
		return -1;
	}
	if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
	{
		return -2;
	}
	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
	{
		return -3;
	}
	
	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,iSize);

	(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset)+iSize;

	(UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) = (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) + iSize;

	return iSize;
}

int truncate_File(char*name)
{
	int fd = GetFDFromName(name);
	if(fd == -1)
	{
		return -1;
	}

	if((UFDTArr[fd].ptrfiletable->writeoffset) != 0)
	{
		memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,(UFDTArr[fd].ptrfiletable->writeoffset)+1);
	
		UFDTArr[fd].ptrfiletable->readoffset = 0;
		UFDTArr[fd].ptrfiletable->writeoffset = 0;
		UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize = 0;
	}
	return 0;
}

int CreateFile(char*name,int Permission)
{
	int i = 0;
	PINODE temp = head;
	
	if((name == NULL)||(Permission == 0)||(Permission > 3))
	{
		return -1;
	}
	if(SUPERBLOCKobj.FreeInodes == 0)
	{
		return -2;
	}
	if((Get_Inode(name)) != NULL)
	{
		return -3;
	}
	(SUPERBLOCKobj.FreeInodes)--;

	while(temp != NULL)
	{
		if(temp->FileType == 0)
		{
			break;
		}
		temp = temp->next;
	}
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		{
			break;
		}
		i++;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable == NULL)
	{
		return -4;
	}

	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = Permission;
	UFDTArr[i].ptrfiletable->readoffset;	
	UFDTArr[i].ptrfiletable->writeoffset = 0;

	UFDTArr[i].ptrfiletable->ptrinode = temp;
	strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->ActualFileSize = 0;
	UFDTArr[i].ptrfiletable->ptrinode->Permission = Permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);
	
	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);

	return i;
}

int OpenFile(char*name,int mode)
{
	int i = 0;
	PINODE temp = NULL;
	
	if((name == NULL)||(mode <= 0)||(mode > 3))
	{
		return -1;
	}
	
	temp = Get_Inode(name);
	if(temp == NULL)
	{
		return -2;
	}
	if((temp->Permission)<mode)
	{
		return -3;
	}
	while(i < 50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		{
			break;
		}
		i++;
	}
	
	if(i == 50)
	{
		return -4;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	
	if(UFDTArr[i].ptrfiletable == NULL)
	{
		return -1;
	}
	
	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = mode;

	if(mode == READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	}
	else if(mode == READ)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
	}
	else if(mode == WRITE)
	{
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	}
	
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

	return i;
}

int ReadFile(int fd, char*arr, int iSize)
{
	int read_size = 0;

	if((UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) == 0)
	{
		return 0;
	}
	
	if(((UFDTArr[fd].ptrfiletable->mode) != READ) && ((UFDTArr[fd].ptrfiletable->mode) != READ+WRITE))
	{
		return -2;
	}
	
	if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != READ) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != READ+WRITE))
	{
		return -2; 
	}
	
	if((UFDTArr[fd].ptrfiletable->readoffset) == (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize))
	{
		return -3;
	}
	
	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
	{
		return -4;
	}

	read_size = (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) - (UFDTArr[fd].ptrfiletable->readoffset);
	
	if(read_size < iSize)
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);
		(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + (read_size);
		
		return read_size;
	}
	else
	{
		strncpy(arr,((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset)),iSize);
		
		(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + iSize;
	}
}

int LseekFile(int fd,int iSize,int from)
{
	if((from < 0)||(from > 2))
	{
		return -1;
	}
		
	if((UFDTArr[fd].ptrfiletable->mode == READ)||(UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
	{
		if(from == CURRENT)	
		{
			if(((UFDTArr[fd].ptrfiletable->readoffset)+iSize) > (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize))
			{
				return -1;
			}
			(UFDTArr[fd].ptrfiletable->readoffset) = ((UFDTArr[fd].ptrfiletable->readoffset)+iSize);
		}
		else if(from == START)
		{
			if(iSize > (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize))
			{
				return -1;
			}
			if(iSize < 0)
			{
				return -1;
			}
			(UFDTArr[fd].ptrfiletable->readoffset) = iSize;
		}
		else if(from == END)
		{
			if(((UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize)+iSize) > MAXFILESIZE)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->readoffset)+iSize)<0)
			{
				return -1;
			}
			(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize)+iSize;
		}
	}
	else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
	{
		if(from == CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+iSize)>MAXFILESIZE)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+iSize) < 0)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+iSize) < 0)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+iSize) > ((UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize)))
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) = (UFDTArr[fd].ptrfiletable->writeoffset)+iSize;
			}
			
			(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset)+iSize;
		}
		else if(from == START)
		{
			if(iSize > MAXFILESIZE)
			{
				return -1;
			}
			if(iSize < 0)
			{
				return -1;
			}
			if(iSize > (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize))
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) = iSize;
			}
		
			(UFDTArr[fd].ptrfiletable->writeoffset) = iSize;
		}
		else if(from == END)
		{
			if(((UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) + iSize) > MAXFILESIZE)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset) + iSize) < 0)
			{
				return -1;
			}
			
			(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize)+iSize;
			(UFDTArr[fd].ptrfiletable->ptrinode->ActualFileSize) = (UFDTArr[fd].ptrfiletable->writeoffset);
		}
	}
	return 0;
}

int main()
{
	char*ptr = NULL;
	int ret = 0, fd = 0, count = 0;
	char command[4][80];
	char str[80],arr[1024];

	InitializeSuperBlock();
	CreateDILB();

	while(1)
	{
		fflush(stdin);
		strcpy(str,"");

		printf("Shweta VFS :>");
		fgets(str,80,stdin);

		count = sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

		if(count == 1)
		{
			if(strcmp(command[0],"ls") == 0)
			{
				ls_File();
			}
			else if(strcmp(command[0],"closeall") == 0)
			{
				CloseAllFiles();
				printf("All files are successfully closed\n");
				continue;
			}
			else if(strcmp(command[0],"clear") == 0)
			{
				system("cls");
				continue;
			}
			else if(strcmp(command[0],"exit") == 0)
			{
				printf("Terminating the Shweta's VFS\n");
				break;
			}
			else
			{
				printf("\nERROR : Command Not Found!!!\n");
				continue;
			}
		}
		else if(count == 2)
		{
			if(strcmp(command[0],"stat") == 0)
			{
				ret = StatFile(command[1]);
				if(ret == -1)
				{
					printf("ERROR : There is no such file\n");
				}
				if(ret == -2)
				{
					printf("ERROR : There is no such file\n");
				}
				continue;
			}
			else if(strcmp(command[0],"fstat") == 0)
			{
				ret = fstat_file(atoi(command[1]));
				if(ret == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
				}
				if(ret == -2)
				{
					printf("ERROR : Thereis no such file\n");
					continue;
				}
			}
			else if(strcmp(command[0],"close") == 0)
			{
				ret = CloseFileByName(command[1]);
				if(ret == -1)
				{
					printf("ERROR : Thereis no such file\n");
				}
				continue;
			}
			else if(strcmp(command[0],"rm") == 0)
			{
				ret = rm_File(command[1]);
				if(ret == -1)
				{
					printf("ERROR : There is no such file\n");
				}
				continue;
			}
			else if(strcmp(command[0],"man") == 0)
			{
				Man(command[1]);
				continue;
			}
			else if(strcmp(command[0],"write") == 0)
			{
				fd = GetFDFromName(command[1]);
				if(fd == -1)
				{
					printf("ERROR : Incorrect parameters\n");
					continue;
				}
				printf("Enter data :\n");
				scanf("%[^\n]",arr);
				
				ret = strlen(arr);
				if(ret == 0)
				{
					printf("Incorrect parameters\n");
					continue;
				}
				ret = strlen(arr);
				if(ret == 0)
				{
					printf("Incorrect parameters\n");
					continue;
				}
				ret = WriteFile(fd,arr,ret);
				if(ret == -1)
				{
					printf("ERROR : Permission denied\n");
				}
				if(ret == -2)
				{
					printf("ERROR : There is no sufficient memory\n");
				}
				if(ret == -3)
				{
					printf("ERROR : It is not regular file\n");
				}
			}
			else if(strcmp(command[0],"truncate") == 0)
			{
				ret = truncate_File(command[1]);
				if(ret == -1)
				{
					printf("ERROR : Incorrect parameters\n");
				}
				else
				{
					printf("\nERROR : Command Not Found!!!\n");
					continue;
				}
			}
		}
			
		else if(count == 3)
		{
			if(strcmp(command[0],"create") == 0)
			{
				ret = CreateFile(command[1],atoi(command[2]));
				if(ret >= 0)
				{
					printf("File is successfully created with fd %d\n",ret);
				}	
				if(ret == -1)
				{
					printf("ERROR : There are no inodes\n");
				}
				else
				{
					printf("\nERROR : Command Not Found!!!\n");
					continue;
				}
			}
		}
		
		else if(count == 4)
		{
			if(strcmp(command[0],"lseek") == 0)
			{
				fd = GetFDFromName(command[1]);
				if(fd == -1)
				{
					printf("ERROR : Incorrect parameters\n");
					continue;
				}
				ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));

				if(ret == -1)	
				{
					printf("ERROR : Unable to perform lseek\n");
				}
			}
			else
			{
				printf("\nERROR : Command Not Found!!!\n");
				continue;
			}
		}
		else
		{
			printf("\nERROR : Command Not Found!!!\n");
			continue;
		}
	}

	return 0;
}
