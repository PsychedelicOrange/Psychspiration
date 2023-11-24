#define ENABLE_VHACD_IMPLEMENTATION 1
#include <VHACD.h>
#include <HullBuilder.h>
#include <FileIO.h>
#include <RModelManager.h>
#include <FloatMath.h>
#include <iostream>
void HullBuilder::builder(string path, std::vector<Hull*>& hulls)
{
	std::string path_hull = pathResource + "/Models/" + path + "_hull"+".gltf";
	if (checkFileExists(path_hull))
	{
		RModelManager* modelMan = RModelManager::getInstance();
		RModel* model = modelMan->getModel(path + "_hull");
		std::cout << model->meshes.size();
		for (RMesh* mesh : model->meshes)
		{
			Hull* hull = new Hull();
			hull->mesh = mesh;
			hulls.push_back(hull);
		}
	}
	else
	{
		std::cout << "Hulls not found! @ "<< path_hull;
	}
}
//void HullBuilder::builder(string path, std::vector<Hull*>& hulls)
//{
//    std::string path_hull = pathResource + "/Hulls/" + path + "_hulls.stl";
//	if(false)//if (checkFileExists(path_hull))
//	{
//        //RModelManager* modelMan = RModelManager::getInstance();
//        //RModel* model = modelMan->getModel(path + "_hulls.stl");
//        
//	}
//	else {
//        //generate hulls
//        RModelManager* modelMan = RModelManager::getInstance();
//        RModel* model = modelMan->getModel(path);
//        std::vector<float> points;
//        vector<unsigned int> indices;
//        for (auto mesh : model->meshes)
//        {
//            for (auto const& vertex : mesh->vertices)
//            {
//                points.push_back(vertex.Position.x);
//                points.push_back(vertex.Position.y);
//                points.push_back(vertex.Position.z);
//            }
//            indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());
//        }
//        //iface->Compute(&points[0],)
//        // 
//        //generate VHACD hulls for each mesh
//        VHACD::IVHACD* iface = VHACD::CreateVHACD();
//        VHACD::IVHACD::Parameters params;
//        params.m_maxConvexHulls = 32;
//        params.m_resolution = 100000;
//        params.m_shrinkWrap = true;
//        params.m_maxNumVerticesPerCH = 32;
//        params.m_minimumVolumePercentErrorAllowed = 0.001;
//            
//        iface->Compute(&points[0], points.size() /3 , indices.data(), indices.size() / 3, params);
//        while (!iface->IsReady())
//        {
//            // computing ...
//        }
//
//        if (!iface->GetNConvexHulls())
//            std::cout << "Failed to produce Convex Hulls : " << iface->GetNConvexHulls();
//        else
//        {
//            //std::cout << model->path << "'s mesh computed !\n";
//            for (int i = 0; i < iface->GetNConvexHulls(); i++)
//            {
//                Hull* hull = new Hull();
//                VHACD::IVHACD::ConvexHull T_hull;
//                iface->GetConvexHull(i, T_hull);
//                for (const auto &point : T_hull.m_points)
//                {
//                    vector<double> pointemp;
//                    pointemp.push_back(point.mX);
//                    pointemp.push_back(point.mY);
//                    pointemp.push_back(point.mZ);
//                    hull->points.push_back(pointemp);
//                }
//                for (const auto &tri : T_hull.m_triangles)
//                {
//                    hull->triangles.push_back(tri.mI0);
//                    hull->triangles.push_back(tri.mI1);
//                    hull->triangles.push_back(tri.mI2);
//                }
//                hulls.push_back(hull);
//            }
//            string path_hull = (pathResource + "/Hulls/" + path + "_hulls.stl").c_str();
//            //std::cout << "Path to Hull " << path_hull;
//            FILE* fph = fopen(path_hull.c_str(), "wb");
//            if (fph)
//            {
//                printf("Generated %i Huls", iface->GetNConvexHulls());
//                printf("Saving convex hull results to a single file \n");
//                for (uint32_t i = 0; i < iface->GetNConvexHulls(); i++)
//                {
//                    VHACD::IVHACD::ConvexHull ch;
//                    iface->GetConvexHull(i, ch);
//                    uint32_t baseIndex = 1;
//                    if (fph)
//                    {
//                        char hullName[2048];
//                        snprintf(hullName, sizeof(hullName), "%s%03d", path.c_str(), i);
//                        fprintf(fph, "solid %s\n", hullName);
//                        for (uint32_t j = 0; j < ch.m_triangles.size(); j++)
//                        {
//                            uint32_t i1 = ch.m_triangles[j].mI0;
//                            uint32_t i2 = ch.m_triangles[j].mI1;
//                            uint32_t i3 = ch.m_triangles[j].mI2;
//
//                            const VHACD::Vertex& p1 = ch.m_points[i1];
//                            const VHACD::Vertex& p2 = ch.m_points[i2];
//                            const VHACD::Vertex& p3 = ch.m_points[i3];
//
//                            double normal[3];
//                            FLOAT_MATH::fm_computePlane((double*)&p1,
//                                (double*)&p2,
//                                (double*)&p3,
//                                normal);
//                            fprintf(fph, " facet normal %0.9f %0.9f %0.9f\n", normal[0], normal[1], normal[2]);
//                            fprintf(fph, "  outer loop\n");
//                            fprintf(fph, "   vertex %0.9f %0.9f %0.9f\n", p1.mX, p1.mY, p1.mZ);
//                            fprintf(fph, "   vertex %0.9f %0.9f %0.9f\n", p2.mX, p2.mY, p2.mZ);
//                            fprintf(fph, "   vertex %0.9f %0.9f %0.9f\n", p3.mX, p3.mY, p3.mZ);
//                            fprintf(fph, "  endloop\n");
//                            fprintf(fph, " endfacet\n");
//                        }
//                        fprintf(fph, "endsolid %s\n", hullName);
//                    }
//                }
//                fclose(fph);
//            }
//            iface->Release();
//        }
//    }
//	
//}