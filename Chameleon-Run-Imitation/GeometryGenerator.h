#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace GeometryGenerator
	{
		struct Vertex
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT3 tangent;
			XMFLOAT2 uv;

			Vertex() :
				pos(XMFLOAT3(0.0f, 0.0f, 0.0f)),
				normal(XMFLOAT3(0.0f, 0.0f, 0.0f)),
				tangent(XMFLOAT3(0.0f, 0.0f, 0.0f)),
				uv(XMFLOAT2(0.0f, 0.0f)) {}
			Vertex(
				const XMFLOAT3& pos_in,
				const XMFLOAT3& normal_in,
				const XMFLOAT3& tangent_u_in,
				const XMFLOAT2& tex_uv_in
			) :
				pos(pos_in),
				normal(normal_in),
				tangent(tangent_u_in),
				uv(tex_uv_in) {}
			Vertex(
				float pos_x, float pos_y, float pos_z,
				float normal_x, float normal_y, float normal_z,
				float tangent_u_x, float tangent_u_y, float tangent_u_z,
				float tex_u, float tex_v
			) :
				pos(pos_x, pos_y, pos_z),
				normal(normal_x, normal_y, normal_z),
				tangent(tangent_u_x, tangent_u_y, tangent_u_z),
				uv(tex_u, tex_v) {}
		};

		struct Geometry
		{
			std::vector<Vertex> vertices;
			std::vector<UINT> indices;
		};

		void CreateBox(float width, float height, float depth, Geometry& mesh_data);
		void CreateSphere(float radius, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data);
		void CreateGeosphere(float radius, UINT subdivision_cnt, Geometry& mesh_data);
		void CreateCylinder(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data);
		void CreateGrid(float width, float depth, UINT x_slice_cnt, UINT z_slice_cnt, Geometry& mesh_data);
		void CreateFullscreenQuad(Geometry& mesh_data);

		void Subdivide(Geometry& mesh_data);
		void BuildCylinderTopCap(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data);
		void BuildCylinderBottomCap(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data);
	}
}
