	/*!
	\class YkLicenseBase
	\brief
	许可信息类。该类用于获得许可的相关信息，连接与断开许可，验证许可是否有效。
*/
#ifndef YkLICENSEBASE_H
#define YkLICENSEBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Stream/YkDefines.h"
// #include "Stream/Ykplatform.h"
// #include "Toolkit/YkLicenseCore.h"

enum ConnectMode
{	
	All					=	0,
	HLOnly				=	1,	
	LocalOnly			=	2,
	RemoteOnly			=	3,
	SpecifiedHostname	=	4,
	SpecifiedIP			=	5,
	SpecifiedKeyID		=	6
};


#include <vector>
#include <string.h>
using namespace std;
namespace Yk
{			
	class TOOLKIT_API YkLicFeatureInfo
	{	
	public:
		YkLicFeatureInfo();
		~YkLicFeatureInfo();

		YkInt m_id;	
		YkString m_name;
		YkBool m_isTrial;
		YkInt m_maxlogins;
		YkString m_connectmode;
		YkBool m_vmenable;
		YkTime m_startTime;
		YkTime m_expiredTime;
		YkUlong m_keyid;
		YkString m_keyType;
		string m_licData;
		YkString m_signature;
		YkInt m_watermarkMode;
		YkString m_userTrademark;
	};
	/*!
	\class YkLicenseBase
	\brief
	许可信息类。该类用于获得许可的相关信息，连接与断开许可，验证许可是否有效。
*/
	class TOOLKIT_API YkLicenseBase
	{
	public:

	protected:
		YkLicenseBase();
	public:	
		virtual ~YkLicenseBase();


		virtual YkInt Connect(YkInt featureID)=0;
		virtual  YkInt Verify()=0;
		virtual void Disconnect()=0;

		// 获取许可相关系信息
		virtual YkLicFeatureInfo GetFeatureInfo() = 0;
	protected:
//_AIX		// 设置许可连接模式，如HLOnly仅本机、SpecifiedIP指定IP，SpecifiedKeyID指定锁号等
		virtual  ConnectMode GetConnectMode();
		virtual  void SetConnectMode(YkInt mode){};

		// 设置连接的服务器地址，除了LocalOnly、RemoteOnly两种模式以外都有对应的用处
		// 如HLOnly时可以设置指定的IP或者计算机名，SpecifiedIP时设置指定IP，
		// SpecifiedKeyID指定锁号、SpecifiedHostname指定计算机名
		virtual  YkString GetConnectServer(){return YkString(_U(""));};
		virtual  void SetConnectServer(YkString& server){};

	private:
		virtual YkString GetUser()=0;
		virtual YkString GetCompany()=0;
		

		// 设置连接错误时的重试次数，为了许可连接效率默认为1次连接，
		// 如果要使用在Internet或通信不好的网络，适当增加一些次数
		virtual  YkInt GetConnectTolerantCount() =0;




	};	
}
#endif //YkLICENSE
