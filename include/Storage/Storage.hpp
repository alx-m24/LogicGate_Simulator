#pragma once

#include "Handle.hpp"

#include <vector>
#include <cstdint>

template<typename T, typename HandleRep = uint32_t>
class Storage {
    public:
        using Handle_T = Handle<T, HandleRep>;

    private:
        std::vector<HandleRep> m_freeList{}; // free slots in the array
        std::vector<HandleRep> m_generations;
        std::vector<T> m_elements{}; // actual array

    public:
        Storage() = default;

    public:
        bool isValid(const Handle_T& h) const {
            return h.isValid() && h.ID < m_generations.size() && m_generations[h.ID] == h.GENERATION;
        }

    public:
        [[nodiscard]] Handle_T AddElement(const T& val) {
            Handle_T handle{};
            if (!m_freeList.empty()) {
                handle.ID = m_freeList.back();
                handle.GENERATION = m_generations[handle.ID];
                
                m_freeList.pop_back();
            }
            else {
                handle.ID = m_elements.size();
                handle.GENERATION = 0;

                m_elements.emplace_back();
                m_generations.push_back(0);
            }

            m_elements[handle.ID] = val;

            return handle;
        }

        [[nodiscard]] Handle_T AddElement(T&& val) {
            Handle_T handle{};
            if (!m_freeList.empty()) {
                handle.ID = m_freeList.back();
                handle.GENERATION = m_generations[handle.ID];
                
                m_freeList.pop_back();
            }
            else {
                handle.ID = m_elements.size();
                handle.GENERATION = 0;

                m_elements.emplace_back();
                m_generations.push_back(0);
            }

            m_elements[handle.ID] = std::move(val);

            return handle;
        }

    public:
        void RemoveElement(const Handle_T& handle) {
            if (isValid(handle)) {
                m_freeList.push_back(handle.ID);
                m_generations[handle.ID] += 1;
            }
        }

    public:
        const T& get(const Handle_T& handle) const {
            if (handle.isValid()) {
                return m_elements[handle.ID];
            }
        }

        T& get(const Handle_T& handle) {
            if (handle.isValid()) {
                return m_elements[handle.ID];
            }
        }

        const T& operator[](const Handle_T& handle) const {
            return get(handle);
        }

        T& operator[](const Handle_T& handle) {
            return get(handle);
        }

    public:

};
