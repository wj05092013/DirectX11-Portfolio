#pragma once

namespace ba
{
	namespace light
	{
		struct DirectionalLight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 direction;
			float pad;

			DirectionalLight();
		};

		struct PointLight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 pos;
			float range;

			XMFLOAT3 attenuation;
			float pad;

			PointLight();
		};

		struct SpotLight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 pos;
			float range;

			XMFLOAT3 direction;
			float spot;

			XMFLOAT3 attenuation;
			float pad;

			SpotLight();
		};

		struct Material
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;
			XMFLOAT4 reflection;

			Material();
		};
	}
}
