/*
 *
 *YkLicenseRSA.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */
#ifndef LICENSERSA_H_
#define  LICENSERSA_H_


#include "Stream/YkDefines.h"
#include "Toolkit/YkLicenseBase.h"
using namespace Yk;

//typedef unsigned char Byte;
typedef unsigned char Byte_;
typedef unsigned int UInt32_;
typedef YkLong Int64_;
typedef YkUlong UInt64_;
using namespace std;

//////////////////////////////////////////////////////////////////////////
// LicenseWrap 文件型许可验证类型，使用自己的算法，与Hasp无关，主要处理Hasp无法运行的情况
// Copyright (c) 2013 SuperMap Software Co., Ltd.
// by gouyu 2013-12-16
//////////////////////////////////////////////////////////////////////////
namespace Yk
{
	class YkLicFeatureInfo;

class  TOOLKIT_API YkLicenseRSA :public YkLicenseBase
{
public:	
	YkLicenseRSA();
	~YkLicenseRSA() override;


	YkInt Connect(YkInt featureID) override;
	YkInt Verify() override;

	void Disconnect() override;

	static bool FindLicFile();

	// 获取许可相关系信息
	YkLicFeatureInfo GetFeatureInfo() override;

	// 指定查找的文件许可位置，可以是指定的文件，也可以是指定的查找目录
	void SetSpecifyLicenseFilePath(YkString& path);

private:
	YkString GetUser() override;
	YkString GetCompany() override;


	// 设置连接错误时的重试次数，为了许可连接效率默认为1次连接，
	// 如果要使用在Internet或通信不好的网络，适当增加一些次数
	YkInt GetConnectTolerantCount() override;
	YkInt VerifyLicFromFile(YkInt featureID);
	static YkInt VerifyDecryptData(string& decryptData, YkString& signature, YkLicFeatureInfo& featureInfo);
	static string Decrypt(string& data);
	static bool VerifyMACAddress(YkString& macAddress);
	static bool VerifyComputerName(YkString& computerName);
	static void FillMacAddressList();
	void FindAndReadConfig();
	//RSA
	// 指定查找的文件许可位置，可以是指定的文件，也可以是指定的查找目录
	YkString GetSpecifyLicenseFilePath();

	

	// 获取当前使用的文件许可的位置
	 YkString GetCurrentLicFilePath();

	static YkString VERSION_NUM;
	static YkString g_currentLicFilePath; // 当前验证的许可文件路径
	static YkString g_specifyLicenseFilePath; // 指定的许可文件 或 去查找许可文件的目录
	static vector<YkString> g_macAddressList;

	YkString m_user;
	YkString m_company;
	YkLicFeatureInfo m_featureInfo;


protected:		
	YkBool canUseLicWrap;
	static YkBool g_configLoaded;
};

// 从C#代码中反编译而来，只用的最关键的modPow方法，其他的都是为了把这个方法编译过才移过来的
class TOOLKIT_API BigInteger
{
	// Methods
public:
	BigInteger(vector<Byte_>& inData);
	
	BigInteger modPow(BigInteger& exp, BigInteger& n);
	vector<Byte_> getBytes();

	BigInteger();
	BigInteger(Int64_ value);
	BigInteger(const BigInteger& bi);
	BigInteger(vector<UInt32_>& inData);
	YkInt bitCount();
	BigInteger BarrettReduction(BigInteger x, BigInteger n, BigInteger constant);
	static void singleByteDivide(BigInteger& bi1, BigInteger& bi2, BigInteger& outQuotient, BigInteger& outRemainder);
	static void multiByteDivide(BigInteger& bi1, BigInteger& bi2, BigInteger& outQuotient, BigInteger& outRemainder);
	static YkInt shiftLeft(vector<UInt32_>& buffer, YkInt shiftVal);
	static YkInt shiftRight(vector<UInt32_>& buffer, YkInt shiftVal);

	BigInteger operator + (BigInteger& bi2);
	BigInteger operator - (BigInteger& bi2);
	BigInteger operator * (BigInteger& bi2);
	BigInteger operator / (BigInteger& bi2);
	BigInteger operator % (BigInteger& bi2);
	BigInteger operator ! (); // 就是取负号，因为和减冲突，所以改成这个
	BigInteger operator << (YkInt shiftVal);
	bool operator >= (BigInteger& bi2);
	bool operator > (BigInteger& bi2);

	// Fields
private:
	vector<UInt32_> data;
	YkInt dataLength;
	static const YkInt maxLength = 70;
};

class LCBase64
{
public:

    /*编码
    DataByte
        [in]输入的数据长度,以字节为单位
    */
    static string Encode(const unsigned char* Data,YkInt DataByte);

	static string Encode(vector<Byte_> data);

    /*解码
    DataByte
        [in]输入的数据长度,以字节为单位
    OutByte
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算
        输出数据的长度
    */
    vector<Byte_> Decode(const char* Data,YkInt DataByte,YkInt& OutByte);

	vector<Byte_> Decode(string& data,YkInt& OutByte);
};
}
#endif // !LicenseWrap_H_

