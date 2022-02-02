////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2021 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_GRAPHICSBATCH_HPP
#define SFML_GRAPHICSBATCH_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/PrimitiveInfo.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>
#include <unordered_map>


namespace sf
{
class Batchable;
class VertexArray;

class GraphicsBatch
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
    GraphicsBatch();

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void begin(RenderTarget& target);

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void begin(RenderTarget& target, BlendMode& blendMode,
               Transform& transform, const Shader* shader);

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void batch(const PrimitiveInfo& batchable, const Texture* texture,
               const Transform& transform, int order);

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void batch(const Vertex* vertices, std::size_t vertexCount,
               PrimitiveType type, const Texture* texture,
               const Transform& transform, int order);

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void end();

private:

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void drawBatches();

    ////////////////////////////////////////////////////////////
    /// TODO
    ////////////////////////////////////////////////////////////
    void sendBatch(std::vector<sf::Vertex>& batch, const sf::Texture* texture);

    struct TriangleInfo
    {
        Vertex   vertices[3];
        const Texture* texture;
        int      drawOrder;
        int      objectOrder;

        TriangleInfo() :
            vertices{},
            texture(nullptr),
            drawOrder(0),
            objectOrder(0)
        {
        }

        TriangleInfo(Vertex a, Vertex b, Vertex c, const Texture* texture,
                     int drawOrder, int preferredOrder) :
            vertices{ a, b, c },
            texture(texture),
            drawOrder(drawOrder),
            objectOrder(preferredOrder)
        {
        }
    };

    std::unordered_map<Texture*, std::vector<TriangleInfo>> m_textureSort;
    std::vector<TriangleInfo> m_triangleInfos;
    std::vector<Vertex>       m_vertices;
    RenderTarget* m_target;
    BlendMode                 m_blendMode;
    Transform                 m_transform;
    const Shader* m_shader;
    bool                      m_inBatch;
    int                       m_nextObjectOrder;
};

} // namespace sf


#endif // SFML_VERTEXBATCH_HPP


////////////////////////////////////////////////////////////
/// TODO
////////////////////////////////////////////////////////////
