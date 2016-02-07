// Homework.cpp : Defines the entry point for the console application.

// NAME: NAREN NAGARAJAN
// ECE 847 ASSIGNMENT 5

#include <afxwin.h>// necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include<math.h>
#include<stack>
#include<vector>
#include<queue>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace blepo;
using namespace std;
int max_disp;
//dissimilarity using absolute difference + convolution
vector<ImgFloat> imagedisp(ImgGray left, ImgGray right)
{ 
	std::vector<ImgFloat> disp(max_disp);
	int i,j;
	int kernal[7];
	for(int i=0;i<7;i++)
		kernal[i]=1;
	int window=7;
	int halfwindow=(window+1)/2;
	ImgFloat temp(left.Width(),left.Height());
	Set(&temp,0);
	for(int d=0;d<max_disp;d++)
	{
		disp[d].Reset(left.Width(),left.Height());
		Set(&disp[d],0);
		for(j=0;j<right.Height()-1;j++)
			for(i=0;i<right.Width()-1;i++)
				if(!(i-d<0))
					disp[d](i,j)=abs(left(i,j)-right(i-d,j));
		for(int y=0;y<left.Height();y++)
			for(int x=halfwindow;x<left.Width()-halfwindow;x++)
			{
				int val=0;
				for(int i=0;i<window;i++)
					val+=kernal[i]*disp[d](x+halfwindow-i,y);
				temp(x,y)=val;
			}
		for(int y=halfwindow;y<left.Height()-halfwindow;y++)
			for(int x=0;x<left.Width();x++)
			{
				int val1=0;
				for(int i=0;i<window;i++)
					val1+=kernal[i]*temp(x,y+halfwindow-i);
				disp[d](x,y)=val1;
			}
	}
	return disp;
}
//disparity map checking various disparites for a given pixel
ImgFloat disparity_map(ImgGray left, ImgGray right)
{
	ImgFloat disparity;
	disparity.Reset(left.Width(),left.Height());
	vector<ImgFloat> dbar;
	dbar=imagedisp(left, right);
	for(int y=0;y<left.Height();y++)
		for(int x=0;x<left.Width();x++)
		{
			int min=9999;
			for(int d=0;d<max_disp;d++)
				if(dbar[d](x,y)<min)
				{
					min=dbar[d](x,y);
					disparity(x,y)=d;
				}
		}
	return disparity;
}
//Left Right Consistency Check using efficient block matching
ImgFloat efficientblockmatch(ImgGray left, ImgGray right)
{
	vector<ImgFloat> dbar;
	dbar=imagedisp(left, right);
	ImgFloat final(left.Width(),left.Height());
	Set(&final,0);
	ImgFloat final1(left.Width(),left.Height());
	Set(&final1,0);
	for(int y=0;y<left.Height();y++)
		for(int x=0;x<left.Width();x++)
		{
			int min=9999;
			for(int d=0;d<max_disp;d++)
				if(dbar[d](x,y)<min)
				{
					min=dbar[d](x,y);
					final(x,y)=d;
				}
		}
	for(int y=0;y<left.Height();y++)
		for(int x=0;x<left.Width();x++)
		{
			int min1=9999;
			for(int d=0;d<max_disp;d++)
				if((dbar[d](x-final(x,y)+d,y))<min1)
				{
					min1=dbar[d](x-final(x,y)+d,y);
					final1(x,y)=d;
				}
		}
	return final1;
}
int main(int argc, const char* argv[], const char* envp[])
{
	// Initialize MFC and return if failure
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
		return 1;
	}
	try
	{
		if(argc==4)
		{
			int i,j,t,x,y;
			string filename; 
			string ext="../../images/";
			filename=ext+argv[1];
			ImgBgr finalimg;
			Load(filename.c_str(), &finalimg);
			Figure origleft("Original Image Left");
			origleft.Draw(finalimg);
			ImgGray imageleft;
			Load(filename.c_str(), &imageleft);
			filename=ext+argv[2];
			ImgBgr bgrright;
			Load(filename.c_str(), &bgrright);
			ImgGray imageright;
			Load(filename.c_str(), &imageright);
			Figure origright("Original Image Right");
			origright.Draw(bgrright);
			ImgGray image;
			Load(filename.c_str(), &image);
			max_disp=atof(argv[3]);
//Disparity Map
			ImgFloat disparity=disparity_map(imageleft,imageright);
			Figure disparity_image("Disparity Map");
			disparity_image.Draw(disparity);
//Efficient Block matching for left-right consistency check
			ImgFloat right_left(imageleft.Width(),imageleft.Height());
			Set(&right_left,0);
			right_left=efficientblockmatch(imageleft,imageright);
			ImgFloat final(imageleft.Width(),imageleft.Height());
			Set(&final,0);
			for(int j=0;j<imageleft.Height();j++)
				for(int i=0;i<imageleft.Width();i++)
					if(right_left(i,j)==disparity(i,j))
						final(i,j)=disparity(i,j);
			Figure finalimage("Left Right Consistency Check");
			finalimage.Draw(final);
// Depth Image
			ImgFloat depth(imageleft.Width(),imageleft.Height());
            int k=1100;
            for (int y=0;y<imageleft.Height();y++)
				for (int x=0;x<imageleft.Width();x++)                                         
					if(final(x,y)>0)
						depth(x,y)=k/final(x,y);
			Figure depthimg("Depth Image");
			depthimg.Draw(depth);
			cout<<"ply file is found in Homework folder as assg5_Naren.ply"<<endl;
//nonzero pixel count
            int count = 0;
            for (int y = 0; y < imageleft.Height(); y++)
				for (int x = 0; x < imageleft.Width(); x++)
					if(final(x,y) != 0)
						count=count+1;
			ofstream final_Naren;
            final_Naren.open("assg5_Naren.ply");
            final_Naren<<"ply"<<endl<<"format ascii 1.0"<<endl<<"element vertex "<<count<<endl<<"property float x"<<endl<<"property float y"<<endl<<"property float z"<<endl<<"property uchar diffuse_red"<<endl<<"property uchar diffuse_green"<<endl<<"property uchar diffuse_blue"<<endl<<"end_header"<<endl;                           
            for (int y=0;y<imageleft.Height();y++)
				for (int x=0; x<imageleft.Width();x++)
					if(depth(x,y)!=0)
						final_Naren<<x<<" "<<-y<<" "<<-depth(x,y)<<" "<<(int)finalimg(x,y).r<<" "<<(int)finalimg(x,y).g<<" "<<(int)finalimg(x,y).b<<endl;                                                                                                    
            final_Naren.close();
		}
		else cout<<"\n Number of Arguments not equal to 3";
		EventLoop();
	}
	catch(const Exception &p)
	{
		p.Display();
	}
	return 0;
}
