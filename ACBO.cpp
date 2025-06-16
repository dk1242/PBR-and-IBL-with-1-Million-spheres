#include "ACBO.h"

ACBO::ACBO()
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ID);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); // Unbind after initialization
}

void ACBO::Reset() const
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ID);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void ACBO::Bind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
}

void ACBO::Unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ACBO::Delete() const
{
    glDeleteBuffers(1, &ID);
}
