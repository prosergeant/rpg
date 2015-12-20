#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>

bool send = false, coord = false, rm = false, load = false;
float oX = 0, oY = 0, oZ = 0, sX = 0, sY = 0, sZ = 0;
int ix = 0, texID = 0, nModel = 0;

void console_update()
{
	char buff[256];
	float nil = 0;
	//static bool run = true;
	if(st == CONSOLE && c_on == true)
	{
	while(c_run){
		gets(buff);
		//printf("buff is %s\n", buff);

		FILE *f = fopen("test.txt", "w+");
		//fputs(buff, f);
		//fclose(f);
		//f = fopen("test.txt", "r");
		
		fprintf(f, "%s", buff);
		rewind(f);
		
		while(c_run){
		int res = fscanf(f, "%s", buff);
		if(res == EOF) break;

		if(strcmp(buff, "tgm") == 0)
		{
			fscanf(f, "%i\n", &tgm);
		}
		else if(strcmp(buff, "tcl") == 0)
		{
			fscanf(f, "%i\n", &tcl);
		}
		else if(strcmp(buff, "end") == 0)
		{
			printf("__console_exit\n");
			st = GAME;
			c_run = false;
		}
		else if(strcmp(buff, "pos") == 0)
		{
			fscanf(f, "%f %f %f\n", &position.x, &position.y, &position.z);
		}
		else if(strcmp(buff, "angle") == 0)
		{
			fscanf(f, "%f %f\n", &verticalAngle, &horizontalAngle);
		}
		else if(strcmp(buff, "create") == 0) 
		{
			send = true;
		}
		else if(strcmp(buff, "create-pos") == 0)
		{
			fscanf(f, "%i %f %f %f %i %f %f %f\n",&nModel, &oX, &oY, &oZ, &texID, &sX, &sY, &sZ);
			coord = true;
		}
		else if(strcmp(buff, "rm-index") == 0)
		{
			fscanf(f, "%i\n", &ix);
			rm = true;	//objs.erase(objs.begin()+index);
		}
		}
		fclose(f);
		remove("test.txt");

		//printf("god mode is : %i\n", tgm);
		//printf("tcl is : %i\n", tcl);
	}
	}

	//return 0;
}
