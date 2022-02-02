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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "GraphicsBatch.hpp"
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>


namespace sf
{
////////////////////////////////////////////////////////////
GraphicsBatch::GraphicsBatch() :
    m_triangleInfos(),
    m_target(NULL),
    m_inBatch(false),
    m_nextObjectOrder(0)
{
    m_triangleInfos.reserve(512);
    m_vertices.reserve(2048);
}


////////////////////////////////////////////////////////////
void GraphicsBatch::begin(RenderTarget& target)
{
    if (m_inBatch)
    {
        err() << "SpriteBatch cannot begin a new batch since another batch is active. Did you forget to call end()?";
        return;
    }

    m_target = &target;
    m_blendMode = sf::BlendAlpha;
    m_transform = sf::Transform::Identity;
    m_shader = nullptr;
    m_inBatch = true;
    m_nextObjectOrder = 0;
}


////////////////////////////////////////////////////////////
void GraphicsBatch::begin(RenderTarget& target, BlendMode& blendMode,
                          Transform& transform, const Shader* shader)
{
    if (m_inBatch)
    {
        err() << "SpriteBatch cannot begin a new batch since another batch is active. Did you forget to call end()?";
        return;
    }

    m_target = &target;
    m_blendMode = blendMode;
    m_transform = transform;
    m_shader = shader;
    m_inBatch = true;
    m_nextObjectOrder = 0;
}


////////////////////////////////////////////////////////////
void GraphicsBatch::batch(const PrimitiveInfo& batchable, const Texture* texture,
                          const Transform& transform, int order)
{
    if (!m_inBatch)
    {
        err() << "GraphicsBatch is not active. Did you forget to call begin()?";
        return;
    }

    batchable.forEachPrimitive(
        [&](const Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type)
        {
            batch(vertices, vertexCount, type, texture, transform, order);
        }
    );
}


////////////////////////////////////////////////////////////
void GraphicsBatch::batch(const Vertex* vertices, std::size_t vertexCount,
                          PrimitiveType type, const Texture* texture,
                          const Transform& transform, int order)
{
    if (!m_inBatch)
    {
        err() << "GraphicsBatch is not active. Did you forget to call begin()?";
        return;
    }

    size_t lastTriangleCount = m_triangleInfos.size();
    int objectOrder = m_nextObjectOrder++;

    if (type == PrimitiveType::TriangleStrip)
    {
        for (std::size_t i = 2; i < vertexCount; i++)
        {
            m_triangleInfos.emplace_back(
                vertices[i - 2],
                vertices[i - 1],
                vertices[i],
                texture, order, objectOrder);
        }
    }
    else if (type == PrimitiveType::Triangles)
    {
        for (int i = 0; i < vertexCount - 2; i += 3)
        {
            m_triangleInfos.emplace_back(
                vertices[i],
                vertices[i + 1],
                vertices[i + 2],
                texture, order, objectOrder);
        }
    }
    else if (type == PrimitiveType::TriangleFan)
    {
        for (std::size_t i = 2; i < vertexCount; i++)
        {
            m_triangleInfos.emplace_back(
                vertices[0],
                vertices[i - 1],
                vertices[i],
                texture, order, objectOrder);
        }
    }
    else
    {
        err() << "This drawable is not supported!";
        return;
    }

    for (size_t i = lastTriangleCount; i < m_triangleInfos.size(); i++)
    {
        TriangleInfo& info = m_triangleInfos[i];

        info.vertices[0].position = transform.transformPoint(info.vertices[0].position);
        info.vertices[1].position = transform.transformPoint(info.vertices[1].position);
        info.vertices[2].position = transform.transformPoint(info.vertices[2].position);
    }
}


////////////////////////////////////////////////////////////
void GraphicsBatch::end()
{
    if (!m_inBatch)
    {
        err() << "SpriteBatch cannot end the batch since there is no active batch. Did you forget to call begin()?";
        return;
    }

    drawBatches();

    m_target = NULL;
    m_inBatch = false;
}


////////////////////////////////////////////////////////////
void GraphicsBatch::drawBatches()
{
    if (m_triangleInfos.size() == 0)
    {
        // Where are my triangles?!
        return;
    }

    static const auto comparer = [](const TriangleInfo& left, const TriangleInfo& right) -> bool
    {
        if (left.drawOrder < right.drawOrder)
            return true;  // left should draw before right

        if (left.drawOrder > right.drawOrder)
            return false;  // right should draw after left

        // Triangles with same order are grouped by texture
        if (left.texture->getNativeHandle() < right.texture->getNativeHandle())
            return true;

        if (left.texture->getNativeHandle() > right.texture->getNativeHandle())
            return false;

        return left.objectOrder < right.objectOrder;
    };

    std::sort(m_triangleInfos.begin(), m_triangleInfos.end(), comparer);

    m_vertices.push_back(m_triangleInfos[0].vertices[0]);
    m_vertices.push_back(m_triangleInfos[0].vertices[1]);
    m_vertices.push_back(m_triangleInfos[0].vertices[2]);

    for (int i = 1; i < m_triangleInfos.size(); i++)
    {
        TriangleInfo& current = m_triangleInfos[i];
        TriangleInfo& last = m_triangleInfos[i - 1];

        if (current.texture != last.texture)
            sendBatch(m_vertices, last.texture);

        m_vertices.push_back(current.vertices[0]);
        m_vertices.push_back(current.vertices[1]);
        m_vertices.push_back(current.vertices[2]);
    }

    if (m_triangleInfos.size() > 1)
    {
        // Send the very last batch
        sendBatch(m_vertices, m_triangleInfos.rbegin()->texture);
    }

    m_triangleInfos.resize(0);
    m_vertices.resize(0);
}

////////////////////////////////////////////////////////////
void GraphicsBatch::sendBatch(std::vector<Vertex>& batch, const Texture* texture)
{
    RenderStates state(m_blendMode, m_transform, texture, m_shader);

    m_target->draw(batch.data(), batch.size(), PrimitiveType::Triangles, state);

    batch.resize(0);
}
} // namespace sf
