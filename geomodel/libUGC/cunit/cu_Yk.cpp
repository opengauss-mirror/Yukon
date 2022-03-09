#include <CUnit/Basic.h>
#include "cu_tester.h"
#include <cfloat>
#include "Geometry3D/YkGeoModel.h"
#include "Geometry3D/YkWrapCGeoModel.h"
#include "Stream/YkFileStream.h"
// 包含其他需要测试函数的头文件

using namespace std;
using namespace Yk;

YkGeoModel *m_pGeoModelPro = NULL;
YkModelSkeleton *m_pSkeleton = NULL;
YkModelMaterial *m_pMaterial = NULL;
YkModelTexture *m_pTexture = NULL;

void InitGeoModel(YkString strFilePath)
{
	YkFileStream fileStreamLoad;
	YkBool bSuccessful = fileStreamLoad.Open(strFilePath, YkStreamLoad);

	m_pGeoModelPro = LoadShellFromStream(fileStreamLoad);
	m_pSkeleton = (YkModelSkeleton *)LoadElementFromStream(fileStreamLoad);
	m_pMaterial = (YkModelMaterial *)LoadElementFromStream(fileStreamLoad);
	m_pTexture = (YkModelTexture *)LoadElementFromStream(fileStreamLoad);

	fileStreamLoad.Close();
}

void ReleaseGeoModel()
{
	delete m_pGeoModelPro;
	m_pGeoModelPro = NULL;

	delete m_pSkeleton;
	m_pSkeleton = NULL;

	delete m_pMaterial;
	m_pMaterial = NULL;

	delete m_pTexture;
	m_pTexture = NULL;
}

void TestVersion()
{
	YkInt nVersion = m_pGeoModelPro->GetVersion();
	YKASSERT(nVersion == 800);
}

void TestSpherePlaced()
{
	YkBool bSpherePlaced = m_pGeoModelPro->GetSpherePlaced();
	YKASSERT(bSpherePlaced == TRUE);
}

void TestPosition()
{
	YkPoint3D position = m_pGeoModelPro->GetPosition();
	YkPoint3D compPosition(113.44486789589689, 22.198337215206539, 5.0000000000000000);
	YKASSERT(position == compPosition);
}

void TestScale()
{
	YkPoint3D scale = m_pGeoModelPro->GetScale();
	YkPoint3D compScale(1.0, 1.0, 1.0);
	YKASSERT(scale == compScale);
}

void TestRotation()
{
	YkPoint3D rotation = m_pGeoModelPro->GetRotation();
	YkPoint3D compRotation(0.0, 0.0, 0.0);
	YKASSERT(rotation == compRotation);
}

void TestBoundingBox()
{
	YkBoundingBox boundingBox = m_pGeoModelPro->GetBoundingBox();
	YkVector3d vMin(-4.1359141266199249, -5.3964246475882192, 20.557742811801660);
	YkVector3d vMax(4.1359141249727145, 5.3964257419020214, 24.903338962833605);
	YkBoundingBox compBoundingBox(vMin, vMax);
	YKASSERT(boundingBox == compBoundingBox);
}

void TestShells()
{
	std::vector<YkModelPagedLOD *> &vecLOD = m_pGeoModelPro->GetModelNode()->GetPagedLODs();
	YKASSERT(vecLOD.size() == 0);

	YkInt nPatchCount = m_pGeoModelPro->GetModelNode()->GetDataPatcheCount();
	YKASSERT(nPatchCount == 1);

	YkModelPagedPatch *pPatch = m_pGeoModelPro->GetModelNode()->GetDataPatche(0);
	YKASSERT(pPatch->GetLODDistance() == 0.0);
	YKASSERT(pPatch->GetIndex() == 0);
	YKASSERT(pPatch->GetParentPatch() == NULL);
	YKASSERT(pPatch->GetChildCount() == 0);
	std::vector<YkModelGeode *> &vecGeode = pPatch->GetGeodes();
	YKASSERT(vecGeode.size() == 1);

	YkModelGeode *pGeode = vecGeode[0];
	YkMatrix4d matrix = pGeode->GetPosition();
	YkMatrix4d compMatrix(0.99999998537855272, -6.4608273761961331e-05, 0.00015833087655516470, 0.0,
						  6.4572604387708315e-05, 0.99999997253998485, 0.00022527851263107101, 0.0,
						  -0.00015834542706345912, -0.00022526828549999334, 0.99999996209046194, 0.0,
						  881.45830563039840, -1114.1009192422357, 43.165065308010199, 1.0);
	//YKASSERT(matrix == compMatrix);
	std::vector<YkString> vecSkeletonName = pGeode->GetElementNames(YkModelElement::etSkeleton);
	YKASSERT(vecSkeletonName.size() == 1);
	YKASSERT(vecSkeletonName[0].Compare(_U("Building_HK51_BU_BB_074_LOD2-1_0")) == 0);

	std::vector<YkString> vecMaterialName = pGeode->GetElementNames(YkModelElement::etMaterial);
	YKASSERT(vecMaterialName.size() == 1);
	YKASSERT(vecMaterialName[0].Compare(_U("HK51_BU_CC_020.jpgffffffff_ffffffff_0_0.000000_")) == 0);

	std::vector<YkString> vecTextureName = pGeode->GetElementNames(YkModelElement::etTexture);
	YKASSERT(vecTextureName.size() == 1);
	YKASSERT(vecTextureName[0].Compare(_U("HK51_BU_CC_020.jpg")) == 0);
}

void TestSkeleton()
{
	YkString strName = m_pSkeleton->GetName();
	YKASSERT(strName.Compare(_U("Building_HK51_BU_BB_074_LOD2-1_0")) == 0);
	std::vector<YkString> vecMaterialName = m_pSkeleton->GetAtt();
	YKASSERT(vecMaterialName.size() == 1);
	YKASSERT(vecMaterialName[0].Compare(_U("HK51_BU_CC_020.jpgffffffff_ffffffff_0_0.000000_")) == 0);
	YkBoundingBox boundingBox = m_pSkeleton->GetBoundingBox();
	YkVector3d vMin(-885.66882324218750, 1108.6435546875000, -22.717172622680664);
	YkVector3d vMax(-877.39819335937500, 1119.4351806640625, -18.374668121337891);
	YkBoundingBox compBoundingBox(vMin, vMax);
	//YKASSERT(boundingBox == compBoundingBox);
	YkMatrix4d matrix = m_pSkeleton->m_matLocalView;
	YkMatrix4d compMatrix(1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0);
	YKASSERT(matrix == compMatrix);
	YKASSERT(m_pSkeleton->GetVertexCount() == 8);
}

void TestMaterial()
{
	YkDouble dVersion = m_pMaterial->m_dVersion;
	YKASSERT(YKEQUAL(dVersion, 1.2));
	YkString strName = m_pMaterial->GetName();
	YKASSERT(strName.Compare(_U("HK51_BU_CC_020.jpgffffffff_ffffffff_0_0.000000_")) == 0);
	std::vector<YkTechnique *> &vecTechnique = m_pMaterial->mTechniques;
	YKASSERT(vecTechnique.size() == 1);

	YkTechnique *pTechnique = vecTechnique[0];
	YKASSERT(pTechnique != NULL);
	YKASSERT(pTechnique->getNumPasses() == 1);

	YkPass *pPass = pTechnique->getPass(0);
	YKASSERT(pPass != NULL);
	YkString strPassName = pPass->m_strName;
	YKASSERT(strPassName.Compare(_U("HK51_BU_CC_020.jpgffffffff_ffffffff_0_0.000000_")) == 0);
	YKASSERT(pPass->m_enPolygonMode == PM_SOLID);
	YKASSERT(pPass->m_bLightEnabled == TRUE);
	YKASSERT(pPass->m_nMaxSimultaneousLights == 8);
	YKASSERT(pPass->m_nStartLight == 0);
	YKASSERT(pPass->m_bPolygonModeOverrideable == TRUE);
	YKASSERT(YKEQUAL(pPass->m_dPointSize, 1.0));
	YKASSERT(YKEQUAL(pPass->m_dPointMinSize, 0.0));
	YKASSERT(YKEQUAL(pPass->m_dPointMaxSize, 0.0));
	YKASSERT(pPass->m_bPointSpritesEnabled == FALSE);
	YKASSERT(pPass->m_bPointAttenuationEnabled == FALSE);
	YKASSERT(YKEQUAL(pPass->m_PointAttenuationCoeffs[0], 1.0));
	YKASSERT(YKEQUAL(pPass->m_PointAttenuationCoeffs[1], 0.0));
	YKASSERT(YKEQUAL(pPass->m_PointAttenuationCoeffs[2], 0.0));
	YKASSERT(pPass->m_enPointSmoothHintMode == SHM_NONE);
	YKASSERT(pPass->m_enLineSmoothHintMode == SHM_NONE);
	YkColorValue3D ambient = pPass->m_Ambient;
	YkColorValue3D compAmbient;
	YKASSERT(ambient == compAmbient);
	YkColorValue3D diffuse = pPass->m_Diffuse;
	YkColorValue3D compDiffuse;
	YKASSERT(diffuse == compDiffuse);
	YkColorValue3D specular = pPass->m_Specular;
	YkColorValue3D compSpecular(0.0, 0.0, 0.0, 0.0);
	YKASSERT(specular == compSpecular);
	YkColorValue3D selfIllumination = pPass->m_SelfIllumination;
	YkColorValue3D compSelfIllumination(0.0, 0.0, 0.0, 0.0);
	YKASSERT(selfIllumination == compSelfIllumination);
	YkColorValue3D materialColor = pPass->m_MaterialColor;
	YkColorValue3D compMaterialColor;
	YKASSERT(materialColor == compMaterialColor);
	YKASSERT(YKEQUAL(pPass->m_Shininess, 0.0));
	YKASSERT(pPass->m_Tracking == 0);
	YKASSERT(pPass->m_bReceiveShadow == FALSE);
	YKASSERT(pPass->m_bColorWrite == TRUE);
	YKASSERT(YKEQUAL(pPass->m_fAlphaReject, 0.0));
	YKASSERT(pPass->m_AlphaRejectFunc == CMPF_ALWAYS_PASS);
	YKASSERT(pPass->m_bAlphaToCoverageEnabled == FALSE);
	YKASSERT(pPass->m_TransparentSorting == FALSE);
	YKASSERT(pPass->m_bTransparentSortingForced == FALSE);
	YKASSERT(pPass->m_bDepthCheck == TRUE);
	YKASSERT(pPass->m_bDepthWrite == TRUE);
	YKASSERT(pPass->m_DepthBufferFunc == CMPF_LESS_EQUAL);
	YKASSERT(YKEQUAL(pPass->m_fConstantPolygonOffset, 0.0));
	YKASSERT(YKEQUAL(pPass->m_fSlopeScalePolygonOffset, 0.0));
	YKASSERT(YKEQUAL(pPass->m_fDepthBiasPerIteration, 0.0));
	YKASSERT(pPass->m_bBlendAlpha == TRUE);
	YKASSERT(pPass->m_strVertexProgram.IsEmpty());
	YKASSERT(pPass->m_strFragmentProgram.IsEmpty());
	YKASSERT(pPass->m_strGeometryProgram.IsEmpty());
	YKASSERT(pPass->m_strShadowCasterVertexProgram.IsEmpty());
	YKASSERT(pPass->m_strShadowReceiverVertexProgram.IsEmpty());
	YKASSERT(pPass->m_strShadowReceiverFragmentProgram.IsEmpty());
	YKASSERT(pPass->GetTextureUnitStatesSize() == 1);
	YKASSERT(pPass->m_nTextureZType[0] == 0);

	YkTextureUnitState *pUnitState = pPass->GetTextureUnitState(0);
	YKASSERT(pUnitState != NULL);
	YkString strUnitStateName = pUnitState->m_strName;
	YKASSERT(strUnitStateName.Compare(_U("0")) == 0);
	YKASSERT(pUnitState->m_strTextureNameAlias.IsEmpty());
	YkString strTextureName = pUnitState->m_strTextureName;
	YKASSERT(strTextureName.Compare(_U("HK51_BU_CC_020.jpg")) == 0);
	YKASSERT(pUnitState->m_strCubicTextureName.IsEmpty());
	YKASSERT(pUnitState->m_strCubicTextureName.IsEmpty());
	YKASSERT(pUnitState->m_unTextureCoordSetIndex == 0);
	YkTextureUnitState::UVWAddressingMode addressMode = pUnitState->m_AddressMode;
	YKASSERT(addressMode.u == YkTextureUnitState::TAM_WRAP);
	YKASSERT(addressMode.v == YkTextureUnitState::TAM_WRAP);
	YKASSERT(addressMode.w == YkTextureUnitState::TAM_WRAP);
	YKASSERT(pUnitState->m_MinFilter == FO_LINEAR);
	YKASSERT(pUnitState->m_MaxFilter == FO_LINEAR);
	YKASSERT(pUnitState->m_MipFilter == FO_POINT);
	YKASSERT(YKEQUAL(pUnitState->m_dUScale, 1.0));
	YKASSERT(YKEQUAL(pUnitState->m_dVScale, 1.0));
	YKASSERT(pUnitState->m_bEnvironmentMapEnabled == FALSE);
	YKASSERT(pUnitState->m_EnvironmentMap == YkTextureUnitState::ENV_PLANAR);
	YkMatrix4d matrix = pUnitState->m_TexModMatrix;
	YkMatrix4d compMatrix(1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0);
	YKASSERT(matrix == compMatrix);
}

void TestTexture()
{
	YkString strName = m_pTexture->GetName();
	YKASSERT(strName.Compare(_U("HK51_BU_CC_020.jpg")) == 0);
	YKASSERT(m_pTexture->m_bMipmap == 1);
	YKASSERT(m_pTexture->m_nLevel == 9);
	YKASSERT(m_pTexture->m_pTextureData != NULL);

	YkTextureData *pTextureData = m_pTexture->m_pTextureData;
	YKASSERT(pTextureData->m_CompressType == 14);
	YKASSERT(pTextureData->m_nWidth == 512);
	YKASSERT(pTextureData->m_nHeight == 512);
	YKASSERT(pTextureData->m_enFormat == PF_A8B8G8R8);
	YKASSERT(pTextureData->m_nSize == 349552);
}

void TestSaveGeoModel(YkString strResultFilePath)
{
	if (YkFile::IsExist(strResultFilePath))
	{
		YkFile::Delete(strResultFilePath);
	}

	YkFileStream fileStreamSave;
	YkBool bS = fileStreamSave.Open(strResultFilePath, YkStreamSave);

	SaveShell2Stream(m_pGeoModelPro, fileStreamSave);
	SaveElement2Stream(m_pSkeleton, fileStreamSave);
	SaveElement2Stream(m_pMaterial, fileStreamSave);
	SaveElement2Stream(m_pTexture, fileStreamSave);

	fileStreamSave.Close();

	YKASSERT(YkFile::IsExist(strResultFilePath));
}

/**
 * @brief 开始测试前需要准备的环境
 * 
 * @return int 
 */
static int init_example_suite(void)
{

	return 0;
}

/**
 * @brief 测试结束后，需要清理环境
 * 
 * @return int 
 */
static int clean_example_suite(void)
{
	return 0;
}

/**
 * @brief 测试示例函数
 * 
 * @param a 
 * @param b 
 * @return int 
 */
static int _max(int a, int b)
{
	return a > b ? a : b;
}

static void test_max(void)
{
	YkString strAppPath = _U("./");
	YkString strFilePath = strAppPath + _U("TestData/testStream.txt");
	YkString strResultFilePath = strAppPath + _U("TestData/resultStream.txt");

	InitGeoModel(strFilePath);
	TestVersion();
	TestSpherePlaced();
	TestPosition();
	TestScale();
	TestRotation();
	TestBoundingBox();
	TestShells();
	TestSkeleton();
	TestMaterial();
	TestTexture();
	TestSaveGeoModel(strResultFilePath);
	ReleaseGeoModel();

	InitGeoModel(strResultFilePath);
	TestVersion();
	TestSpherePlaced();
	TestPosition();
	TestScale();
	TestRotation();
	TestBoundingBox();
	TestShells();

	TestSkeleton();
	TestMaterial();
	TestTexture();
	ReleaseGeoModel();
}

void example_suite_setup(void);
void example_suite_setup(void)
{
	CU_pSuite suite = CU_add_suite("example_test", init_example_suite, clean_example_suite);
	YK_ADD_TEST(suite, test_max);
}