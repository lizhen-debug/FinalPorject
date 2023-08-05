#pragma once

enum TextureType
{
    map_Kd,     //������������ͼ
    map_Bump,   //������ͼ
    map_Ke,     //�Է���������ͼ
    map_Ka,     //������������ͼ
    map_Ks,     //���淴��������ͼ
    map_Ns,     //�߹��������ͼ
    map_d,      //͸����������ͼ
    map_Tr,     //͸����������ͼ�������ʽ��
    map_disp,   //λ��������ͼ
    map_refl,   //����������ͼ
};

class Material
{
public:

    string name;
    float Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ke[3];
    float Ni;
    float d;
    int illum;
    map<TextureType, TCHAR*> res_type_path; // �洢���ʵ�������ͼ·��

	Material();
	~Material();

private:

};

Material::Material()
{
}

Material::~Material()
{
}