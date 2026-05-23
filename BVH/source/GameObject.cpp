#include "GameObject.h"

GameObject::GameObject(sf::FloatRect _rect)
{
    Initialise();
    SetPosition({_rect.left, _rect.top});
    SetSize({_rect.width, _rect.height});
}

void GameObject::Initialise()
{
    // Init the vertex array
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(4);

    // Create a set of points for the vertex
    m_vertices[0].position = sf::Vector2f(0, 0);
    m_vertices[1].position = sf::Vector2f(1, 0);
    m_vertices[2].position = sf::Vector2f(1, 1);
    m_vertices[3].position = sf::Vector2f(0, 1);

    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
    // Set the colour of the vertex
    sf::Color color = sf::Color(rR, rG, rB);
    m_vertices[0].color = color;
    m_vertices[1].color = color;
    m_vertices[2].color = color;
    m_vertices[3].color = color;
}

void GameObject::SetPosition(const sf::Vector2f _position)
{
    m_position = _position;
    m_collider.UpdateBoundingBoxPos(m_position);
    // Debug only
    auto& boundingBox = m_collider.GetBoundingBox();
    m_vertices[0].position = sf::Vector2f(m_position.x, m_position.y);
    m_vertices[1].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y);
    m_vertices[2].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y + boundingBox.height);
    m_vertices[3].position = sf::Vector2f(m_position.x, m_position.y + boundingBox.height);
}

void GameObject::IncrementPosition(sf::Vector2f _position)
{
    m_position += _position;
    m_collider.UpdateBoundingBoxPos(m_position);

    // Debug only
    auto& boundingBox = m_collider.GetBoundingBox();
    m_vertices[0].position = sf::Vector2f(m_position.x, m_position.y);
    m_vertices[1].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y);
    m_vertices[2].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y + boundingBox.height);
    m_vertices[3].position = sf::Vector2f(m_position.x, m_position.y + boundingBox.height);
}

void GameObject::SetSize(const sf::Vector2f _size)
{
    m_collider.CreateBoundingBox(_size);
    // Debug only
    auto& boundingBox = m_collider.GetBoundingBox();
    m_vertices[0].position = sf::Vector2f(m_position.x, m_position.y);
    m_vertices[1].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y);
    m_vertices[2].position = sf::Vector2f(m_position.x + boundingBox.width, m_position.y + boundingBox.height);
    m_vertices[3].position = sf::Vector2f(m_position.x, m_position.y + boundingBox.height);
}

void GameObject::SetColor(sf::Color _color)
{
    m_vertices[0].color = _color;
    m_vertices[1].color = _color;
    m_vertices[2].color = _color;
    m_vertices[3].color = _color;
}

sf::Vector2f GameObject::GetPosition() const
{
    return m_position;
}

sf::Vector2f GameObject::GetSize()
{
    return { m_collider.GetBoundingBox().width, m_collider.GetBoundingBox().height };
}

Collider& GameObject::GetCollider()
{
    return m_collider;
}
sf::VertexArray& GameObject::GetVertexArray()
{
    return m_vertices;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_vertices, states);
}
