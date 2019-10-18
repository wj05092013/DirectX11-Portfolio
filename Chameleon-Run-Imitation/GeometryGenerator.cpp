#include "stdafx.h"

void ba::GeometryGenerator::CreateBox(float width, float height, float depth, Geometry& mesh_data)
{
	// Create the vertices.
	//
	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	mesh_data.vertices.assign(&v[0], &v[24]);
	//__

	// Create the indices.
	//
	UINT i[36];

	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh_data.indices.assign(&i[0], &i[36]);
	//__
}

void ba::GeometryGenerator::CreateSphere(float radius, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data)
{
	mesh_data.vertices.clear();
	mesh_data.indices.clear();

	// Compute the vertices stating at the top pole and moving down the stacks.
	//
	Vertex top_vertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottom_vertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	mesh_data.vertices.push_back(top_vertex);

	float phi_step = XM_PI / stack_cnt;
	float theta_step = 2.0f * XM_PI / slice_cnt;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stack_cnt - 1; ++i)
	{
		float phi = i * phi_step;

		// Vertices of ring.
		for (UINT j = 0; j <= slice_cnt; ++j)
		{
			float theta = j * theta_step;

			Vertex v;

			// spherical to cartesian
			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = +radius * sinf(phi) * cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.tangent);
			XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;

			mesh_data.vertices.push_back(v);
		}
	}

	mesh_data.vertices.push_back(bottom_vertex);
	//__

	// Compute indices for top stack.
	for (UINT i = 1; i <= slice_cnt; ++i)
	{
		mesh_data.indices.push_back(0);
		mesh_data.indices.push_back(i + 1);
		mesh_data.indices.push_back(i);
	}

	// Compute indices for inner stacks (not connected to poles).
	UINT base_idx = 1;
	UINT ring_vertex_cnt = slice_cnt + 1;
	for (UINT i = 0; i < stack_cnt - 2; ++i)
	{
		for (UINT j = 0; j < slice_cnt; ++j)
		{
			mesh_data.indices.push_back(base_idx + i * ring_vertex_cnt + j);
			mesh_data.indices.push_back(base_idx + i * ring_vertex_cnt + j + 1);
			mesh_data.indices.push_back(base_idx + (i + 1) * ring_vertex_cnt + j);

			mesh_data.indices.push_back(base_idx + (i + 1) * ring_vertex_cnt + j);
			mesh_data.indices.push_back(base_idx + i * ring_vertex_cnt + j + 1);
			mesh_data.indices.push_back(base_idx + (i + 1) * ring_vertex_cnt + j + 1);
		}
	}

	// Compute indices for bottom stack.
	//

	// South pole vertex was added last.
	UINT south_pole_idx = (UINT)mesh_data.vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_idx = south_pole_idx - ring_vertex_cnt;

	for (UINT i = 0; i < slice_cnt; ++i)
	{
		mesh_data.indices.push_back(south_pole_idx);
		mesh_data.indices.push_back(base_idx + i);
		mesh_data.indices.push_back(base_idx + i + 1);
	}
	//__
}

void ba::GeometryGenerator::Subdivide(Geometry& mesh_data)
{
	// Save a copy of the input geometry.
	Geometry input_copy = mesh_data;


	mesh_data.vertices.resize(0);
	mesh_data.indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	UINT triangle_cnt = input_copy.indices.size() / 3;
	for (UINT i = 0; i < triangle_cnt; ++i)
	{
		Vertex v0 = input_copy.vertices[input_copy.indices[i * 3 + 0]];
		Vertex v1 = input_copy.vertices[input_copy.indices[i * 3 + 1]];
		Vertex v2 = input_copy.vertices[input_copy.indices[i * 3 + 2]];

		// Generate the midpoints.
		//
		Vertex m0, m1, m2;

		m0.pos = XMFLOAT3(
			0.5f * (v0.pos.x + v1.pos.x),
			0.5f * (v0.pos.y + v1.pos.y),
			0.5f * (v0.pos.z + v1.pos.z));

		m1.pos = XMFLOAT3(
			0.5f * (v1.pos.x + v2.pos.x),
			0.5f * (v1.pos.y + v2.pos.y),
			0.5f * (v1.pos.z + v2.pos.z));

		m2.pos = XMFLOAT3(
			0.5f * (v0.pos.x + v2.pos.x),
			0.5f * (v0.pos.y + v2.pos.y),
			0.5f * (v0.pos.z + v2.pos.z));
		//__

		// Add new geometry.
		//
		mesh_data.vertices.push_back(v0); // 0
		mesh_data.vertices.push_back(v1); // 1
		mesh_data.vertices.push_back(v2); // 2
		mesh_data.vertices.push_back(m0); // 3
		mesh_data.vertices.push_back(m1); // 4
		mesh_data.vertices.push_back(m2); // 5

		mesh_data.indices.push_back(i * 6 + 0);
		mesh_data.indices.push_back(i * 6 + 3);
		mesh_data.indices.push_back(i * 6 + 5);

		mesh_data.indices.push_back(i * 6 + 3);
		mesh_data.indices.push_back(i * 6 + 4);
		mesh_data.indices.push_back(i * 6 + 5);

		mesh_data.indices.push_back(i * 6 + 5);
		mesh_data.indices.push_back(i * 6 + 4);
		mesh_data.indices.push_back(i * 6 + 2);

		mesh_data.indices.push_back(i * 6 + 3);
		mesh_data.indices.push_back(i * 6 + 1);
		mesh_data.indices.push_back(i * 6 + 4);
		//__
	}
}

void ba::GeometryGenerator::CreateGeosphere(float radius, UINT subdivision_cnt, Geometry& mesh_data)
{
	// Put a cap on the number of subdivisions.
	subdivision_cnt = mathhelper::Min(subdivision_cnt, 5u);

	// Approximate a sphere by tessellating an icosahedron.

	const float x = 0.525731f;
	const float z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-x, 0.0f, z),  XMFLOAT3(x, 0.0f, z),
		XMFLOAT3(-x, 0.0f, -z), XMFLOAT3(x, 0.0f, -z),
		XMFLOAT3(0.0f, z, x),   XMFLOAT3(0.0f, z, -x),
		XMFLOAT3(0.0f, -z, x),  XMFLOAT3(0.0f, -z, -x),
		XMFLOAT3(z, x, 0.0f),   XMFLOAT3(-z, x, 0.0f),
		XMFLOAT3(z, -x, 0.0f),  XMFLOAT3(-z, -x, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh_data.vertices.resize(12);
	mesh_data.indices.resize(60);

	for (UINT i = 0; i < 12; ++i)
		mesh_data.vertices[i].pos = pos[i];

	for (UINT i = 0; i < 60; ++i)
		mesh_data.indices[i] = k[i];

	for (UINT i = 0; i < subdivision_cnt; ++i)
		Subdivide(mesh_data);

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < mesh_data.vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mesh_data.vertices[i].pos));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		XMStoreFloat3(&mesh_data.vertices[i].pos, p);
		XMStoreFloat3(&mesh_data.vertices[i].normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = mathhelper::AngleFromXY(
			mesh_data.vertices[i].pos.x,
			mesh_data.vertices[i].pos.z);

		float phi = acosf(mesh_data.vertices[i].pos.y / radius);

		mesh_data.vertices[i].uv.x = theta / XM_2PI;
		mesh_data.vertices[i].uv.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		mesh_data.vertices[i].tangent.x = -radius * sinf(phi) * sinf(theta);
		mesh_data.vertices[i].tangent.y = 0.0f;
		mesh_data.vertices[i].tangent.z = +radius * sinf(phi) * cosf(theta);

		XMVECTOR t = XMLoadFloat3(&mesh_data.vertices[i].tangent);
		XMStoreFloat3(&mesh_data.vertices[i].tangent, XMVector3Normalize(t));
	}
}

void ba::GeometryGenerator::CreateCylinder(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data)
{
	mesh_data.vertices.clear();
	mesh_data.indices.clear();

	// Build Stacks.
	//
	float stack_height = height / stack_cnt;

	//  Amount to increment radius as we move up each stack level from bottom to top.
	float radius_step = (top_radius - bottom_radius) / stack_cnt;

	UINT ring_cnt = stack_cnt + 1;

	//  Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ring_cnt; ++i)
	{
		float y = -0.5f * height + i * stack_height;
		float r = bottom_radius + i * radius_step;

		// vertices of ring
		float d_theta = 2.0f * XM_PI / slice_cnt;
		for (UINT j = 0; j <= slice_cnt; ++j)
		{
			Vertex vertex;

			float c = cosf(j * d_theta);
			float s = sinf(j * d_theta);

			vertex.pos = XMFLOAT3(r * c, y, r * s);

			vertex.uv.x = (float)j / slice_cnt;
			vertex.uv.y = 1.0f - (float)i / stack_cnt;

			// This is unit length.
			vertex.tangent = XMFLOAT3(-s, 0.0f, c);

			float d_r = bottom_radius - top_radius;
			XMFLOAT3 bitangent(d_r * c, -height, d_r * s);

			XMVECTOR t = XMLoadFloat3(&vertex.tangent);
			XMVECTOR b = XMLoadFloat3(&bitangent);
			XMVECTOR n = XMVector3Normalize(XMVector3Cross(t, b));
			XMStoreFloat3(&vertex.normal, n);

			mesh_data.vertices.push_back(vertex);
		}
	}

	UINT ring_vertex_cnt = slice_cnt + 1;
	//__

	// Compute indices for each stack.
	for (UINT i = 0; i < stack_cnt; ++i)
	{
		for (UINT j = 0; j < slice_cnt; ++j)
		{
			mesh_data.indices.push_back(i * ring_vertex_cnt + j);
			mesh_data.indices.push_back((i + 1) * ring_vertex_cnt + j);
			mesh_data.indices.push_back((i + 1) * ring_vertex_cnt + j + 1);

			mesh_data.indices.push_back(i * ring_vertex_cnt + j);
			mesh_data.indices.push_back((i + 1) * ring_vertex_cnt + j + 1);
			mesh_data.indices.push_back(i * ring_vertex_cnt + j + 1);
		}
	}

	BuildCylinderTopCap(bottom_radius, top_radius, height, slice_cnt, stack_cnt, mesh_data);
	BuildCylinderBottomCap(bottom_radius, top_radius, height, slice_cnt, stack_cnt, mesh_data);
}

void ba::GeometryGenerator::BuildCylinderTopCap(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data)
{
	UINT base_idx = (UINT)mesh_data.vertices.size();

	float y = 0.5f * height;
	float d_theta = 2.0f * XM_PI / slice_cnt;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= slice_cnt; ++i)
	{
		float x = top_radius * cosf(i * d_theta);
		float z = top_radius * sinf(i * d_theta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh_data.vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	mesh_data.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Index of center vertex.
	UINT center_idx = (UINT)mesh_data.vertices.size() - 1;

	for (UINT i = 0; i < slice_cnt; ++i)
	{
		mesh_data.indices.push_back(center_idx);
		mesh_data.indices.push_back(base_idx + i + 1);
		mesh_data.indices.push_back(base_idx + i);
	}
}

void ba::GeometryGenerator::BuildCylinderBottomCap(float bottom_radius, float top_radius, float height, UINT slice_cnt, UINT stack_cnt, Geometry& mesh_data)
{
	// 
	// Build bottom cap.
	//

	UINT base_idx = (UINT)mesh_data.vertices.size();
	float y = -0.5f * height;

	// vertices of ring
	float d_theta = 2.0f * XM_PI / slice_cnt;
	for (UINT i = 0; i <= slice_cnt; ++i)
	{
		float x = bottom_radius * cosf(i * d_theta);
		float z = bottom_radius * sinf(i * d_theta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh_data.vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	mesh_data.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Cache the index of center vertex.
	UINT center_idx = (UINT)mesh_data.vertices.size() - 1;

	for (UINT i = 0; i < slice_cnt; ++i)
	{
		mesh_data.indices.push_back(center_idx);
		mesh_data.indices.push_back(base_idx + i);
		mesh_data.indices.push_back(base_idx + i + 1);
	}
}

void ba::GeometryGenerator::CreateGrid(float width, float depth, UINT x_slice_cnt, UINT z_slice_cnt, Geometry& mesh_data)
{
	UINT vertex_cnt = x_slice_cnt * z_slice_cnt;
	UINT face_cnt = (x_slice_cnt - 1) * (z_slice_cnt - 1) * 2;

	//
	// Create the vertices.
	//

	float half_width = 0.5f * width;
	float half_depth = 0.5f * depth;

	float dx = width / (z_slice_cnt - 1);
	float dz = depth / (x_slice_cnt - 1);

	float du = 1.0f / (z_slice_cnt - 1);
	float dv = 1.0f / (x_slice_cnt - 1);

	mesh_data.vertices.resize(vertex_cnt);
	for (UINT i = 0; i < x_slice_cnt; ++i)
	{
		float z = half_depth - i * dz;
		for (UINT j = 0; j < z_slice_cnt; ++j)
		{
			float x = -half_width + j * dx;

			mesh_data.vertices[i * z_slice_cnt + j].pos = XMFLOAT3(x, 0.0f, z);
			mesh_data.vertices[i * z_slice_cnt + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mesh_data.vertices[i * z_slice_cnt + j].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			mesh_data.vertices[i * z_slice_cnt + j].uv.x = j * du;
			mesh_data.vertices[i * z_slice_cnt + j].uv.y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	mesh_data.indices.resize(face_cnt * 3); // 3 indices per face

											// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < x_slice_cnt - 1; ++i)
	{
		for (UINT j = 0; j < z_slice_cnt - 1; ++j)
		{
			mesh_data.indices[k] = i * z_slice_cnt + j;
			mesh_data.indices[k + 1] = i * z_slice_cnt + j + 1;
			mesh_data.indices[k + 2] = (i + 1) * z_slice_cnt + j;

			mesh_data.indices[k + 3] = (i + 1) * z_slice_cnt + j;
			mesh_data.indices[k + 4] = i * z_slice_cnt + j + 1;
			mesh_data.indices[k + 5] = (i + 1) * z_slice_cnt + j + 1;

			k += 6; // next quad
		}
	}
}

void ba::GeometryGenerator::CreateFullscreenQuad(Geometry& mesh_data)
{
	mesh_data.vertices.resize(4);
	mesh_data.indices.resize(6);

	// Position coordinates specified in NDC space.
	mesh_data.vertices[0] = Vertex(
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	mesh_data.vertices[1] = Vertex(
		-1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	mesh_data.vertices[2] = Vertex(
		+1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	mesh_data.vertices[3] = Vertex(
		+1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	mesh_data.indices[0] = 0;
	mesh_data.indices[1] = 1;
	mesh_data.indices[2] = 2;

	mesh_data.indices[3] = 0;
	mesh_data.indices[4] = 2;
	mesh_data.indices[5] = 3;
}