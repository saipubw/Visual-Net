#include"code.h"
//#define Code_DEBUG
#define Show_Scale_Img(src) do\
{\
	Mat temp=ScaleToDisSize(src);\
	imshow("Code_DEBUG", temp);\
	waitKey();\
}while (0);
namespace Code
{
	constexpr int BytesPerFrame = 3738;
	constexpr int FrameSize = 108;
	constexpr int FrameOutputRate = 10;
	constexpr int SafeAreaWidth = 2;
	constexpr int QrPointSize = 18;
	constexpr int SmallQrPointbias = 6;
	constexpr int RectAreaCount = 7;
	const Vec3b pixel[8] = 
	{ 
		Vec3b(0,0,0),Vec3b(0,0,255),Vec3b(0,255,0),Vec3b(0,255,255),
		Vec3b(255,0,0),Vec3b(255,0,255),Vec3b(255,255,0), Vec3b(255,255,255)
	};
	const int lenlim[RectAreaCount] = { 426,432,1944,432,432,48,24 };
	const int areapos[RectAreaCount][2][2] = //[2][2],��һά�ȴ����߿����ڶ�ά�ȴ������Ͻ�����
	{
		{{71,16},{QrPointSize + 1,SafeAreaWidth}},
		{{16,72},{SafeAreaWidth,QrPointSize}},
		{{72,72},{QrPointSize,QrPointSize}},
		{{72,16},{QrPointSize,FrameSize - QrPointSize}},
		{{16,72},{FrameSize - QrPointSize,QrPointSize}},
		{{8,16},{FrameSize  - QrPointSize,FrameSize - QrPointSize}},
		{{8,8},{FrameSize - QrPointSize + 8,FrameSize - QrPointSize}}
	};
	enum color
	{
		Black = 0,
		White = 7
	};
	enum class FrameType
	{
		Start = 0,
		End = 1,
		StartAndEnd = 2,
		Normal = 3
	};
	Mat ScaleToDisSize(const Mat& src)
	{
		Mat dis;
		constexpr int FrameOutputSize = FrameSize * FrameOutputRate;
		dis = Mat(FrameOutputSize, FrameOutputSize, CV_8UC3);
		for (int i = 0; i < FrameOutputSize; ++i)
		{
			for (int j = 0; j < FrameOutputSize; ++j)
			{
				dis.at<Vec3b>(i,j) = src.at<Vec3b>(i/10, j/10);
			}
		}
		return dis;
	}
	uint16_t CalCheckCode(const char* info, int len)
	{
		uint16_t ans = 0;
		int cutlen = len / 2 * 2;
		for (int i = 0; i < cutlen; i += 2)
			ans ^= ((uint16_t)info[i] << 8) | info[i+1];
		if (len & 1)
			ans ^= (uint16_t)info[cutlen]<<8;
		return ans;
	}
	void BulidSafeArea(Mat& mat)
	{
		constexpr int pos[4][2][2] =
		{
			{{0,FrameSize},{0,SafeAreaWidth}},
			{{0,FrameSize},{FrameSize - SafeAreaWidth,FrameSize}},
			{{0, SafeAreaWidth },{0,FrameSize}},
			{{FrameSize - SafeAreaWidth,FrameSize},{0,FrameSize}}
		};
		for (int k=0;k<4;++k)
			for (int i = pos[k][0][0]; i < pos[k][0][1]; ++i)
				for (int j = pos[k][1][0]; j < pos[k][1][1]; ++j)
					mat.at<Vec3b>(i,j)=pixel[White];
#ifdef Code_DEBUG
		Show_Scale_Img(mat);
#endif
		return;
	}
	void BulidQrPoint(Mat& mat)
	{
		//���ƴ��ά��ʶ���
		constexpr int pointPos[4][2] = 
		{ 
			{0,0},
		    {0,FrameSize- QrPointSize},
		    {FrameSize - QrPointSize,0}
		};
		const Vec3b vec3bBig[9] =
		{
			pixel[Black],
			pixel[Black],
			pixel[Black],
			pixel[White],
			pixel[White],
			pixel[Black],
			pixel[Black],
			pixel[White],
			pixel[White]
		};
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < QrPointSize; ++j)
				for (int k = 0; k < QrPointSize; ++k)
					mat.at<Vec3b>(pointPos[i][0] + j, pointPos[i][1] + k) =
						vec3bBig[(int)max(fabs(j-8.5), fabs(k-8.5))];
		//����С��ά��ʶ���
		constexpr int posCenter[2] = { FrameSize - SmallQrPointbias,FrameSize - SmallQrPointbias };
		const Vec3b vec3bsmall[5] =
		{ 
			pixel[Black],
			pixel[Black],
			pixel[White],
			pixel[Black],
			pixel[White],
		};
		for (int i = -4; i <= 4; ++i)
			for (int j = -4; j <= 4; ++j)
				mat.at<Vec3b>(posCenter[0] + i, posCenter[1] + j) = 
							  vec3bsmall[max(abs(i),abs(j))];
#ifdef Code_DEBUG
		Show_Scale_Img(mat);
#endif
	}
	void BulidCheckCodeAndFrameNo(Mat& mat,uint16_t checkcode,uint8_t FrameNo)
	{
		uint32_t outputCode = (checkcode << 8) | (FrameNo);
		for (int i = 8; i < 16; ++i)
		{
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + i) = pixel[outputCode & 7];
			outputCode >>= 3;
		}
#ifdef Code_DEBUG
		Show_Scale_Img(mat);
#endif
	}
	void BulidInfoRect(Mat& mat, const char* info, int len,int areaID)
	{
		const unsigned char* pos = (const unsigned char*)info;
		const unsigned char* end = pos + len;
		for (int i = 0; i < areapos[areaID][0][0]; ++i)
		{
			uint32_t outputCode = 0;
			for (int j = 0; j < areapos[areaID][0][1]/8; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					outputCode <<= 8;
					if (pos != end)
						outputCode |= *pos++;
				}
				for (int k = areapos[areaID][1][1]; k < areapos[areaID][1][1]+8; ++k)
				{
					mat.at<Vec3b>(i+areapos[areaID][1][0], j*8+k) = pixel[outputCode&7];
					outputCode >>= 3;
				}
				if (pos == end) break;
			}
			if (pos == end) break;
		}
#ifdef Code_DEBUG
		Show_Scale_Img(mat);
#endif
	}
	void BulidFrameFlag(Mat& mat, FrameType frameType, int tailLen)
	{
		tailLen = 3123;
		switch (frameType)
		{
		case FrameType::Start:
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[Black];
			break;
		case FrameType::End:
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[White];
			break;
		case FrameType::StartAndEnd:
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[White];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[White];
			break;
		default:
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[Black];
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[Black];
			break;
		}
		for (int i = 4; i < 8; ++i)
		{
			mat.at<Vec3b>(QrPointSize, SafeAreaWidth + i) = pixel[tailLen&7];
			tailLen >>= 3;
		}
#ifdef Code_DEBUG
		Show_Scale_Img(mat);
#endif
	}
	Mat CodeFrame(FrameType frameType, const char* info, int tailLen,int FrameNo)
	{
		Mat codeMat = Mat(FrameSize, FrameSize, CV_8UC3,Vec3d(128,128,128));
		if (frameType != FrameType::End&&frameType!=FrameType::StartAndEnd) 
			tailLen = BytesPerFrame;
		BulidSafeArea(codeMat);
		BulidQrPoint(codeMat);
		int checkCode=CalCheckCode(info, tailLen);
		BulidFrameFlag(codeMat, frameType, tailLen);
		BulidCheckCodeAndFrameNo(codeMat, checkCode, FrameNo % 256);
		for (int i = 0; i < RectAreaCount&&tailLen>0; ++i)
		{
			int lennow = std::min(tailLen, lenlim[i]);
			BulidInfoRect(codeMat, info, lennow,i);
			tailLen -= lennow;
			info += lennow;
		}
		return codeMat;
	}
	void main(const char* info, int len,const char * savePath,const char * outputFormat)
	{
		Mat output;
		char fileName[128];
		int counter = 0;
		if (len <= 0);
		else if (len <= BytesPerFrame)
		{
			output= ScaleToDisSize(CodeFrame(FrameType::StartAndEnd, info, len, 0));
			sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
			imwrite(fileName, output);
		}
		else
		{
			int i = 0;
			len -= BytesPerFrame;
			output= ScaleToDisSize(CodeFrame(FrameType::Start, info, len, 0));
			sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
			imwrite(fileName, output);
			do
			{
				info += BytesPerFrame;
				if (len - BytesPerFrame > 0)
					output= ScaleToDisSize(CodeFrame(FrameType::Normal, info, BytesPerFrame, ++i));
				else
					output= ScaleToDisSize(CodeFrame(FrameType::End, info, len, ++i));
				len -= BytesPerFrame;
				sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
				imwrite(fileName, output);
			} while (len>0);
		}
		return;
	}
}