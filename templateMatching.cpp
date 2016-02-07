// Homework.cpp : Defines the entry point for the console application.

// NAME: NAREN NAGARAJAN
// ECE 847 ASSIGNMENT 3

#include <afxwin.h>// necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include<math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace blepo;

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
		if (argc==3||argc==4)
		{
			// a. input & b. 1d gaussian kernel & gaussian derivative kernel
			double sigma = atof(argv[1]);
			double mu=2.5*sigma-0.5;
			int i,j,x,y;
			float val;
			int w=2*(int)mu+1;
			double* g=(double*)calloc(w,sizeof(double));
			double* gd=(double*)calloc(w,sizeof(double));
			double sumg=0,sumgd=0;
			for(i=0;i<w;i++)
			{
				g[i]=exp(-(i-mu)*(i-mu)/(2*sigma*sigma));
				gd[i]=(i-mu)*g[i];
				sumg=sumg+g[i];
				sumgd=sumgd+(gd[i]*i);
			}
			for(i=0;i<w;i++)
			{
				g[i]=g[i]/sumg;//1d gaussian kernel
				gd[i]=gd[i]/sumgd;//1d gaussian derivative kernel
			}
			cout<<"\n\n Gaussian Kernel :\n";
			for(i=0;i<w;i++)
			{
				cout<<"g["<<i<<"] = "<<g[i]<<"     ";
			}
			cout<<"\n\n Gaussian Derivative Kernel :\n";
			for(i=0;i<w;i++)
			{
				cout<<"gd["<<i<<"] =  "<<gd[i]<<"   ";
			}
			// c. gradient
			ImgGray image;
			ImgFloat tempi;
			ImgFloat tempo;
			ImgFloat tempi1;
			ImgFloat tempo1;
			string filename; 
			string ext="../../images/";
			filename=ext+argv[2];
			Load(filename.c_str(), &image);
			tempi.Reset(image.Width(),image.Height());
			tempo.Reset(image.Width(),image.Height());
			tempi1.Reset(image.Width(),image.Height());
			tempo1.Reset(image.Width(),image.Height());
			Set(&tempi,0);
			Set(&tempi1,0);
			Set(&tempo,0);
			Set(&tempo1,0);
			ImgBgr final;
			Load(filename.c_str(), &final);
			Figure imag;
			imag.Draw(final);
			imag.SetTitle("Original Image");
			//using gaussian kernel 
			ImgFloat imgfloat;
			imgfloat.Reset(image.Width(),image.Height());
			Convert(image,&imgfloat);
			for(y=mu;y<image.Height()-mu;y++)
			{
				for(x=0;x<image.Width();x++)
				{
					val=0;
					for(i=0;i<w;i++)
					{
						val=val+g[i]*imgfloat(x,y+mu-i);
						tempi(x,y)=val;
					}
				}
			}
			for(y=0;y<image.Height();y++)		
			{
				for(x=mu;x<image.Width()-mu;x++)
				{
					val=0;
					for(i=0;i<w;i++)
					{
						val=val+(gd[i]*tempi(x+mu-i,y));
						tempo(x,y)=val;
					}
				}
			}
			Figure blur;
			blur.Draw(tempo);
			blur.SetTitle("Gx : x Gradient");


			for(y=0;y<image.Height();y++)		
			{
				for(x=mu;x<image.Width()-mu;x++)
				{
					val=0;
					for(i=0;i<w;i++)
					{	
						val=val+(g[i]*imgfloat(x+mu-i,y));
						tempi1(x,y)=val;
					}
				}
			}

			for(y=mu;y<image.Height()-mu;y++)
			{
				for(x=0;x<image.Width();x++)
				{
					val=0;
					for(i=0;i<w;i++)
					{
						val=val+gd[i]*tempi1(x,y+mu-i);
						tempo1(x,y)=val;
					}
				}
			}
			Figure smoothe;
			smoothe.Draw(tempo1);
			smoothe.SetTitle("Gy : y Gradient");
			//gradient calculation
			ImgFloat gradmag;
			ImgFloat gradphase;
			gradmag.Reset(image.Width(),image.Height());
			Set(&gradmag,0);
			gradphase.Reset(image.Width(),image.Height());
			Set(&gradphase,0);	
			for(j=0;j<image.Height();j++)
			{
				for(i=0;i<image.Width();i++)
				{
					if(abs(tempo(i,j))>abs(tempo1(i,j)))
						gradmag(i,j)=abs(tempo(i,j));
					else gradmag(i,j)=abs(tempo1(i,j));
					gradphase(i,j)=atan2(tempo1(i,j),tempo(i,j));
				}
			}
			Figure gradmagnitude;
			gradmagnitude.Draw(gradmag);
			gradmagnitude.SetTitle(" Gradient Magnitude");
			Figure gradientphase(" Gradient Phase");
			gradientphase.Draw(gradphase);
			//non maximal suppression
			ImgFloat gradcheck;
			gradcheck.Reset(image.Width(),image.Height());
			Set(&gradcheck,0);
			float pi=3.14;
			for(j=1;j<image.Height()-1;j++)
			{
				for(i=1;i<image.Width()-1;i++)
				{
					if((gradphase(i,j)>=pi/8)&&gradphase(i,j)<3*pi/8) //left diagonal check
					{
						if(gradmag(i-1,j-1)>gradmag(i,j)||gradmag(i+1,j+1)>gradmag(i,j))
						{
							gradcheck(i,j)=0;
						}
						else gradcheck(i,j)=(int)gradmag(i,j);
					}
					else if((gradphase(i,j)>=5*pi/8)&&gradphase(i,j)<7*pi/8) //right diagonal check
					{	if(gradmag(i-1,j+1)>gradmag(i,j)||gradmag(i+1,j-1)>gradmag(i,j))
					{
						gradcheck(i,j)=0;
					}	
					else gradcheck(i,j)=(int)gradmag(i,j);
					}
					else if((gradphase(i,j)>=3*pi/8)&&gradphase(i,j)<5*pi/8) //north-south check
					{
						if(gradmag(i,j-1)>gradmag(i,j)||gradmag(i,j+1)>gradmag(i,j))
						{
							gradcheck(i,j)=0;
						}
						else gradcheck(i,j)=(int)gradmag(i,j);
					}
					else if((gradphase(i,j)>=(-pi/8))&&gradphase(i,j)<pi/8) //west east check
					{
						if(gradmag(i-1,j)>gradmag(i,j)||gradmag(i+1,j)>gradmag(i,j))
						{
							gradcheck(i,j)=0;
						}
						else gradcheck(i,j)=(int)gradmag(i,j);
					}
				}
			}
			Figure gradientcheck("Non Maximal Suppression ");
			gradientcheck.Draw(gradcheck);

			//Histogram
			int hist[255]={0},pixel;
			for(j=0;j<image.Height();j++)
			{
				for(i=0;i<image.Width();i++)
				{
					pixel=(int)gradcheck(i,j);
					hist[pixel]++;
				}
			}
			int m0[256]={0};
			m0[0]=hist[0];
			int m1[256]={0};
			m1[0]=0;
			int t;
			for(i=1;i<255;i++)
			{
				m0[i]=m0[i-1]+hist[i];
				m1[i]=m1[i-1]+(i*hist[i]);
			}
			t=10;
			int q,ww,e,r;
			int mu1,mu0,ttemp;
			do
			{
				ttemp=t;
				q=m1[t];
				ww=m0[t];
				e=m1[254];
				r=m0[254];
				mu0=m1[t]/m0[t];
				mu1=(m1[254]-m1[t])/(m0[254]-m0[t]);
				t=0.5*(mu1+mu0);
			}while(ttemp!=t);
			int hth=t;
			int lth=t/5;
			ImgFloat lowth;
			lowth.Reset(image.Width(),image.Height());
			Set(&lowth,0);
			ImgFloat highth;
			highth.Reset(image.Width(),image.Height());
			Set(&highth,0);
			for(j=0;j<image.Height();j++)
			{
				for(i=0;i<image.Width();i++)
				{
					if(gradcheck(i,j)>lth)
					{
						lowth(i,j)=1;
					}
					if(gradcheck(i,j)>hth)
					{
						highth(i,j)=1;
					}
				}
			}
			// Canny Edge Detection
			ImgFloat doublethreshold;
			doublethreshold.Reset(image.Width(),image.Height());
			Set(&doublethreshold,0);

			for(j=1;j<image.Height()-1;j++)
			{
				for(i=1;i<image.Width()-1;i++)
				{
					if(gradcheck(i,j)>hth)
					{	
						doublethreshold(i,j)=1;
						if(gradcheck(i-1,j-1)>lth)
							doublethreshold(i-1,j-1)=1;
						if(gradcheck(i-1,j)>lth)
							doublethreshold(i-1,j)=1;
						if(gradcheck(i-1,j+1)>lth)
							doublethreshold(i-1,j+1)=1;
						if(gradcheck(i,j-1)>lth)
							doublethreshold(i,j-1)=1;
						if(gradcheck(i,j+1)>lth)
							doublethreshold(i,j+1)=1;
						if(gradcheck(i+1,j-1)>lth)
							doublethreshold(i+1,j-1)=1;
						if(gradcheck(i+1,j)>lth)
							doublethreshold(i+1,j)=1;
						if(gradcheck(i+1,j+1)>lth)
							doublethreshold(i+1,j+1)=1;
					}
				}
			}
			Figure doublethresh("Canny edges of the image");
			doublethresh.Draw(doublethreshold);
			//Chamfering
			ImgInt chamf;
			chamf.Reset(doublethreshold.Width(),doublethreshold.Height());
			for(y=0;y< doublethreshold.Height();y++)
			{
				for(x=0;x<doublethreshold.Width();x++)
				{
					chamf(x,y)=0;
				}
			}
			for(y=1;y< doublethreshold.Height()-1;y++)
			{
				for(x=1;x<doublethreshold.Width()-1;x++)
				{
					if(doublethreshold(x,y)==1)
					{
						chamf(x,y)=0;
					}
					else
					{
						chamf(x,y)= min(1+chamf(x-1,y),1+ chamf(x,y-1) );
					}
				}
			}
			int out=0;
			for(y=doublethreshold.Height()-2;y>=0 ;y--)
			{
				for(x=doublethreshold.Width()-2;x>=0;x--)
				{
					if(doublethreshold(x,y)==1)
					{
						chamf(x,y)=0;
					}
					else
					{
						out = min(1+chamf(x+1,y),chamf(x,y));
						chamf(x,y)= min(out,1+ chamf(x,y+1));
					}
				}
			}
			Figure chamferimg("Chamfering");
			chamferimg.Draw(chamf);
			//*****************************************************************************************************************************
			//Template

			if(argc==4)
			{
				// a. input & b. 1d gaussian kernel & gaussian derivative kernel
				double sigma1 = atof(argv[1]);
				double mu1=2.5*sigma1-0.5;
				int i1,j1,x1,y1;
				float val1;
				int w1=2*(int)mu1+1;
				double* g1=(double*)calloc(w1,sizeof(double));
				double* gd1=(double*)calloc(w1,sizeof(double));
				double sumg1=0,sumgd1=0;
				for(i1=0;i1<w1;i1++)
				{
					g1[i1]=exp(-(i1-mu1)*(i1-mu1)/(2*sigma1*sigma1));
					gd1[i1]=(i1-mu1)*g1[i1];
					sumg1=sumg1+g1[i1];
					sumgd1=sumgd1+(gd1[i1]*i1);
				}
				for(i1=0;i1<w1;i1++)
				{
					g1[i1]=g1[i1]/sumg1;//1d gaussian kernel
					gd1[i1]=gd1[i1]/sumgd1;//1d gaussian derivative kernel
				}
				// c. gradient
				ImgGray image1;
				ImgFloat tempi11;
				ImgFloat tempo11;
				ImgFloat tempi111;
				ImgFloat tempo111;
				string filename1; 
				string ext1="../../images/";
				filename1=ext1+argv[3];
				Load(filename1.c_str(), &image1);
				tempi11.Reset(image1.Width(),image1.Height());
				tempo11.Reset(image1.Width(),image1.Height());
				tempi111.Reset(image1.Width(),image1.Height());
				tempo111.Reset(image1.Width(),image1.Height());
				Set(&tempi11,0);
				Set(&tempi111,0);
				Set(&tempo11,0);
				Set(&tempo111,0);
				//using gaussian kernel 
				ImgFloat imgfloat1;
				imgfloat1.Reset(image1.Width(),image1.Height());
				Convert(image1,&imgfloat1);
				for(y1=mu1;y1<image1.Height()-mu1;y1++)
				{
					for(x1=0;x1<image1.Width();x1++)
					{
						val1=0;
						for(i1=0;i1<w1;i1++)
						{
							val1=val1+g1[i1]*imgfloat1(x1,y1+mu1-i1);
							tempi11(x1,y1)=val1;
						}
					}
				}
				for(y1=0;y1<image1.Height();y1++)
				{
					for(x1=mu1;x1<image1.Width()-mu1;x1++)
					{
						val1=0;
						for(i1=0;i1<w1;i1++)
						{
							val1=val1+(gd1[i1]*tempi11(x1+mu1-i1,y1));
							tempo11(x1,y1)=val1;
						}
					}
				}
				for(y1=0;y1<image1.Height();y1++)
				{
					for(x1=mu1;x1<image1.Width()-mu1;x1++)
					{
						val1=0;
						for(i1=0;i1<w1;i1++)
						{	
							val1=val1+(g1[i1]*imgfloat1(x1+mu1-i1,y1));
							tempi111(x1,y1)=val1;
						}
					}
				}
				for(y1=mu1;y1<image1.Height()-mu1;y1++)
				{
					for(x1=0;x1<image1.Width();x1++)
					{
						val1=0;
						for(i1=0;i1<w1;i1++)
						{
							val1=val1+gd1[i1]*tempi111(x1,y1+mu1-i1);
							tempo111(x1,y1)=val1;
						}
					}
				}
				//gradient calculation
				ImgFloat gradmag1;
				ImgFloat gradphase1;
				gradmag1.Reset(image1.Width(),image1.Height());
				Set(&gradmag1,0);
				gradphase1.Reset(image1.Width(),image1.Height());
				Set(&gradphase1,0);	
				for(j1=0;j1<image1.Height();j1++)
					for(i1=0;i1<image1.Width();i1++)
					{
						if(abs(tempo11(i1,j1))>abs(tempo111(i1,j1)))
							gradmag1(i1,j1)=abs(tempo11(i1,j1));
						else gradmag1(i1,j1)=abs(tempo111(i1,j1));
						gradphase1(i1,j1)=atan2(tempo111(i1,j1),tempo11(i1,j1));
					}
					//non maximal suppression
					ImgFloat gradcheck1;
					gradcheck1.Reset(image1.Width(),image1.Height());
					Set(&gradcheck1,0.000000);
					float pi1=3.14;
					for(j1=1;j1<image1.Height()-1;j1++)
						for(i1=1;i1<image1.Width()-1;i1++)
						{
							if((gradphase1(i1,j1)>=pi/8)&&gradphase1(i1,j1)<3*pi/8) //left diagonal check
							{
								if(gradmag1(i1-1,j1-1)>gradmag1(i1,j1)||gradmag1(i1+1,j1+1)>gradmag1(i1,j1))
									gradcheck1(i1,j1)=0;
								else gradcheck1(i1,j1)=(int)gradmag1(i1,j1);
							}
							else if((gradphase1(i1,j1)>=5*pi/8)&&gradphase1(i1,j1)<7*pi/8) //right diagonal check
							{	if(gradmag1(i1-1,j1+1)>gradmag1(i1,j1)||gradmag1(i1+1,j1-1)>gradmag1(i1,j1))
							gradcheck1(i1,j1)=0;
							else gradcheck1(i1,j1)=(int)gradmag1(i1,j1);
							}
							else if((gradphase1(i1,j1)>=3*pi/8)&&gradphase1(i1,j1)<5*pi/8) //north-south check
							{
								if(gradmag1(i1,j1-1)>gradmag1(i1,j1)||gradmag1(i1,j1+1)>gradmag1(i1,j1))
									gradcheck1(i1,j1)=0;
								else gradcheck1(i1,j1)=(int)gradmag1(i1,j1);
							}
							else if((gradphase1(i1,j1)>=(-pi/8))&&gradphase1(i1,j1)<pi/8) //west east check
							{
								if(gradmag1(i1-1,j1)>gradmag1(i1,j1)||gradmag1(i1+1,j1)>gradmag1(i1,j1))
									gradcheck1(i1,j1)=0;
								else gradcheck1(i1,j1)=(int)gradmag1(i1,j1);
							}
						}
						//Histogram
						int hist1[255]={0},pixel1;
						for(j1=0;j1<image1.Height();j1++)
							for(i1=0;i1<image1.Width();i1++)
							{
								pixel1=(int)gradcheck1(i1,j1);
								hist1[pixel1]++;
							}
							int m01[256]={0};
							m01[0]=hist1[0];
							int m11[256]={0};
							m11[0]=0;
							int t1;
							for(i1=1;i1<255;i1++)
							{
								m01[i1]=m01[i1-1]+hist1[i1];
								m11[i1]=m11[i1-1]+(i1*hist1[i1]);
							}
							t1=10;
							int q1,ww1,e1,r1;
							int mu11,mu01,ttemp1;
							do
							{
								ttemp1=t1;
								q1=m11[t1];
								ww1=m01[t1];
								e1=m11[254];
								r1=m01[254];
								mu01=m11[t1]/m01[t1];
								mu11=(m11[254]-m11[t1])/(m01[254]-m01[t1]);
								t1=0.5*(mu11+mu01);
							}while(ttemp1!=t1);
							int hth1=t1;
							int lth1=t1/5;
							ImgFloat lowth1;
							lowth1.Reset(image1.Width(),image1.Height());
							Set(&lowth1,0);
							ImgFloat highth1;
							highth1.Reset(image1.Width(),image1.Height());
							Set(&highth1,0);
							for(j1=0;j1<image1.Height();j1++)
								for(i1=0;i1<image1.Width();i1++)
								{
									if(gradcheck1(i1,j1)>lth1)
										lowth1(i1,j1)=1;
									if(gradcheck1(i1,j1)>hth1)
										highth1(i1,j1)=1;
								}
								// Double Thresholding
								ImgFloat doublethreshold1;
								doublethreshold1.Reset(image1.Width(),image1.Height());
								Set(&doublethreshold1,0);
								for(j1=1;j1<image1.Height()-1;j1++)
									for(i1=1;i1<image1.Width()-1;i1++)
									{
										if(gradcheck1(i1,j1)>hth1)
										{	
											doublethreshold1(i1,j1)=1;
											if(gradcheck1(i1-1,j1-1)>lth1)
												doublethreshold1(i1-1,j1-1)=1;
											if(gradcheck1(i1-1,j1)>lth1)
												doublethreshold1(i1-1,j1)=1;
											if(gradcheck1(i1-1,j1+1)>lth1)
												doublethreshold1(i1-1,j1+1)=1;
											if(gradcheck1(i1,j1-1)>lth1)
												doublethreshold1(i1,j1-1)=1;
											if(gradcheck1(i1,j1+1)>lth1)
												doublethreshold1(i1,j1+1)=1;
											if(gradcheck1(i1+1,j1-1)>lth1)
												doublethreshold1(i1+1,j1-1)=1;
											if(gradcheck1(i1+1,j1)>lth1)
												doublethreshold1(i1+1,j1)=1;
											if(gradcheck1(i1+1,j1+1)>lth1)
												doublethreshold1(i1+1,j1+1)=1;
										}
									}
									Figure doublethresh1("Template with Canny Edge Detection");
									doublethresh1.Draw(doublethreshold1);
									//Inverse Probability map
									int sumc=0;
									ImgFloat test;
									test.Reset(image.Width(),image.Height());
									Set(&test,0);
									for(j=0;j<image.Height()-image1.Height()-1;j++)
										for(i=0;i<image.Width()-image1.Width()-1;i++)
										{
											j1=j;
											sumc=0;
											for(y=0;y<image1.Height()-1;y++,j1++)
											{
												i1=i;
												for(x=0;x<image1.Width()-1;x++,i1++)
												{
													if(doublethreshold1(x,y)==1)
														sumc=sumc+chamf(i1,j1);
												}
											}
											test(i,j)=sumc;
										}
										Figure figtest("Inverse Probability Map");
										figtest.Draw(test);
										int minvalue=100000000;
										int l1,l2;
										for(j=0;j<image.Height();j++)
											for(i=0;i<image.Width();i++)
											{
												if(test(i,j)!=0)
												{
													if(minvalue>test(i,j))
													{
														minvalue=test(i,j);
														l1=i;
														l2=j;
													}
												}
											}
											for(j=l2;j<l2+image1.Height();j++)
											{
												final(l1,j).r=0;
												final(l1,j).g=255;
												final(l1,j).b=0;
												final(l1+image1.Width(),j).r=0;
												final(l1+image1.Width(),j).g=255;
												final(l1+image1.Width(),j).b=0;
											}
											for(i=l1;i<l1+image1.Width();i++)
											{
												final(i,l2).r=0;
												final(i,l2).g=255;
												final(i,l2).b=0;
												final(i,l2+image1.Height()).r=0;
												final(i,l2+image1.Height()).g=255;
												final(i,l2+image1.Height()).b=0;
											}
											Figure finalimage("Final Image with Template Detection");
											finalimage.Draw(final);
			} 
		}
		else
			cout<<"\n Enter only 2 or 3 Command line Arguments!";
		EventLoop();
	}
	catch(const Exception &p)
	{
		p.Display();
	}
	return 0;
}


