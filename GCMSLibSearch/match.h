#ifdef MATCHAPI

#else
#define  MATCHAPI extern "C" __declspec(dllimport)
#endif

/************************************************************************/
/* ���ܣ���ͼ�Ա�
   ������
   Mass_Match����ƥ��������
   AjustedAbundance_Match����ƥ�����������
   MassCount_Match����ƥ������
   Mass_Lib������ͼ������
   AjustedAbundance_Lib������ͼ���������
   MassCount_Lib������ͼ�����
   ����ֵ��
   -1��ƥ��ʧ��
   ˵���������Mass_Match��Mass_Lib��Ҫ����������С�������С�
*/
/************************************************************************/
MATCHAPI unsigned int
	DiffSpectrum(unsigned int* Mass_Match, float* AjustedAbundance_Match, unsigned int MassCount_Match,
	unsigned int* Mass_Lib, float* AjustedAbundance_Lib, unsigned int MassCount_Lib);