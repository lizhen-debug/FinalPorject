#pragma once

enum TextureType
{
    map_Kd,     //漫反射纹理贴图
    map_Bump,   //法线贴图
    map_Ke,     //自发光纹理贴图
    map_Ka,     //环境光纹理贴图
    map_Ks,     //镜面反射纹理贴图
    map_Ns,     //高光度纹理贴图
    map_d,      //透明度纹理贴图
    map_Tr,     //透明度纹理贴图（替代形式）
    map_disp,   //位移纹理贴图
    map_refl,   //反射纹理贴图
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
    map<TextureType, TCHAR*> res_type_path; // 存储材质的纹理贴图路径

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