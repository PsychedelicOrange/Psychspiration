#pragma once
class Frustum
{
public:
	float halftan, nhh, nhw, fhh, fhw,near_plane,far_plane, diagnolFrustrum;
	glm::vec4 Coords_VS[9]; // View Space Coordinates
	glm::vec4 Coords_WS[9]; // World Space Coordinates
	glm::vec4 Coords_LS[8]; // Light Space Coordinates

	// Renderer 
	float Coords_WS_ARRAY[3 * 9];
	const unsigned int indices[3 * 6] = {
	   0,1,8,// left 
	   1,2,8,// up
	   2,3,8,// right
	   3,0,8,// down
	   4,5,6, // near plane
	   6,7,4 //
	};
	Frustum(float yfov,float aspectratio, float near_plane, float far_plane)
	{
		 halftan = tan(yfov / 2);
		 nhh = halftan * near_plane;
		 nhw = aspectratio * nhh;
		 fhh = (far_plane / near_plane) * nhh;
		 fhw = aspectratio * fhh;
		 this->near_plane = near_plane;
		 this->far_plane = far_plane;

		 updateCoords_VS();
	}
	void updateCoords_VS()
	{
		Coords_VS[0] = (glm::vec4(-fhw, fhh, -far_plane, 1)); //top left 0
		Coords_VS[1] = (glm::vec4(fhw, fhh, -far_plane, 1));// top right 1
		Coords_VS[2] = (glm::vec4(fhw, -fhh, -far_plane, 1)); // bottom right 2
		Coords_VS[3] = (glm::vec4(-fhw, -fhh, -far_plane, 1)); // bottom left 3

		Coords_VS[4] = (glm::vec4(-nhw, nhh, -near_plane, 1)); // top left 4
		Coords_VS[5] = (glm::vec4(nhw, nhh, -near_plane, 1)); //top right 5
		Coords_VS[6] = (glm::vec4(nhw, -nhh, -near_plane, 1)); // bottom right 6
		Coords_VS[7] = (glm::vec4(-nhw, -nhh, -near_plane, 1)); // bottom right 7

		Coords_VS[8] = (glm::vec4(0, 0, 0, 1)); // origin 8
		
		diagnolFrustrum = glm::length(Coords_VS[6] - Coords_VS[0]);
	}
	void constructWS(glm::mat4 ViewInverse)
	{
		for (int i = 0; i < 9; i++)
		{
			Coords_WS[i] = (ViewInverse * Coords_VS[i]);
			Coords_WS_ARRAY[3 * i] = Coords_WS[i].x;
			Coords_WS_ARRAY[3 * i + 1] = Coords_WS[i].y;
			Coords_WS_ARRAY[3 * i + 2] = Coords_WS[i].z;
		}
	}
	void updateWS(float * ptr,glm::mat4 ViewInverse)
	{
		for (int i = 0; i < 9; i++)
		{
			Coords_WS[i] = (ViewInverse * Coords_VS[i]);
			ptr[3 * i] = Coords_WS[i].x;
			ptr[3 * i + 1] = Coords_WS[i].y;
			ptr[3 * i + 2] = Coords_WS[i].z;
		}
	}
	void constructLS(glm::mat4 LightView)
	{
		for (int i = 0; i < 8; i++)
		{
			Coords_LS[i] = LightView * Coords_WS[i];
		}
	}
	float* getFrustrumMinMax()
	{
		float xyminmax[4] = { Coords_LS[0].x,Coords_LS[0].y,Coords_LS[0].x,Coords_LS[0].y };
		for (int i = 1; i < 8; i++)
		{
			if (xyminmax[0] > Coords_LS[i].x)
				xyminmax[0] = Coords_LS[i].x;
			if (xyminmax[1] < Coords_LS[i].x)
				xyminmax[1] = Coords_LS[i].x;

			if (xyminmax[2] > Coords_LS[i].y)
				xyminmax[2] = Coords_LS[i].y;
			if (xyminmax[3] < Coords_LS[i].y)
				xyminmax[3] = Coords_LS[i].y;
		}
		return xyminmax;
	}
};