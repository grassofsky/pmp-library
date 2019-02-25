//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include <pmp/visualization/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceSmoothing.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    virtual void process_imgui();

private:
    SurfaceSmoothing smoother_;
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height),
      smoother_(mesh_)
{
}

//=============================================================================

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.mean_curvature_to_texture_coordinates();
            update_mesh();
            mesh_.use_cold_warm_texture();
            set_draw_mode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Smoothing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int weight = 0;
        ImGui::RadioButton("Cotan Laplace", &weight, 0);
        ImGui::RadioButton("Uniform Laplace", &weight, 1);
        bool uniform_laplace = (weight == 1);

        static int iterations = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Iterations", &iterations, 1, 100);
        ImGui::PopItemWidth();

        if (ImGui::Button("Explicit Smoothing"))
        {
            smoother_.explicit_smoothing(iterations, uniform_laplace);
            update_mesh();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        static float timestep = 0.001;
        float lb = uniform_laplace ? 1.0 : 0.001;
        float ub = uniform_laplace ? 100.0 : 1.0;
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("TimeStep", &timestep, lb, ub);
        ImGui::PopItemWidth();

        if (ImGui::Button("Implicit Smoothing"))
        {
            Scalar dt =
                uniform_laplace ? timestep : timestep * radius_ * radius_;
            smoother_.implicit_smoothing(dt, uniform_laplace);
            update_mesh();
        }
    }
}

//=============================================================================

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Smoothing", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Smoothing", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
