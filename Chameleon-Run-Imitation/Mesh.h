#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Mesh
	{
	public:
		Mesh();
		virtual ~Mesh();

		virtual void Draw(ID3D11DeviceContext* dc) const;

		bool BuildVertexBuffer(ID3D11Device* device, const std::vector<inputvertex::PosNormalTexTangent::Vertex> vertices);

		bool BuildIndexBuffer(ID3D11Device* device, const std::vector<UINT> indices);

		// Mutators.
		void set_local_transform(const XMMATRIX& matrix);

		// Accessors.
		const std::vector<inputvertex::PosNormalTexTangent::Vertex>& vertices() const;
		UINT vertex_stride() const;
		const XMMATRIX& local_transform() const;

	private:
		ID3D11Buffer* vb_;
		UINT vertex_stride_;
		std::vector<inputvertex::PosNormalTexTangent::Vertex> vertices_;

		ID3D11Buffer* ib_;
		UINT idx_count_;
		
		XMMATRIX local_transform_;
	};
}
