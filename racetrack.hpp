#ifndef RACETRACK_HPP
#define RACETRACK_HPP

#include <iostream>
#include <limits>
#include <vector>

class Racetrack
{
public:
    using cell_index_t = unsigned int;

    enum class cell_t
    {
        outside = 0,
        road = 1,
        start = 2,
        finish = 3,
        trace = 4
    };

    cell_t & operator()(int i, int j)
    {
        return m_data[cell(i, j)];
    }

    const cell_t & operator()(int i, int j) const
    {
        return m_data[cell(i, j)];
    }

    cell_t & operator()(cell_index_t c)
    {
        return m_data[c];
    }

    const cell_t & operator()(cell_index_t c) const
    {
        return m_data[c];
    }

    cell_index_t cell(int i, int j) const
    {
        return static_cast<cell_index_t>(i*m_w + j);
    }

    int width() const
    {
        return m_w;
    }

    int height() const
    {
        return m_h;
    }

    int size() const
    {
        return m_w*m_h;
    }

    /*
    const std::array<cell_index_t, 8> & neighbours(cell_index_t c) const
    {
        return m_neighbours[c];
    }
    */

    std::istream & read_ppm(std::istream & is)
    {
        if (is.fail())
        {
            std::cerr << "File is not readable" << std::endl;
            throw std::system_error();
        }

        std::string id;
        is >> id;
        if (id != "P3")
        {
            throw std::runtime_error("Wrong id, P3 expected, but got " + id);
        }

        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        int w, h;
        is >> w >> h;
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        int max_value;
        is >> max_value;
        if (max_value != 255)
        {
            throw std::runtime_error("Max != 255");
        }
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (is.fail())
        {
            throw std::system_error();
        }

        resize(w, h);

        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
            {
                int r, g, b;
                is >> r >> g >> b;
                if (r == 0 && g == 0 && b == 0)
                {
                    (*this)(i, j) = cell_t::road;
                }
                else if (r == 255 && g == 0 && b == 0)
                {
                    (*this)(i, j) = cell_t::start;
                }
                else if (r == 0 && g == 0 && b == 255)
                {
                    (*this)(i, j) = cell_t::finish;
                }
                else if (r == 255 && g == 255 && b == 255)
                {
                    (*this)(i, j) = cell_t::outside;
                }
                else
                {
                    throw std::runtime_error("Unknown value in ppm file");
                }
            }
        return is;
    }

    std::ostream & write_ppm(std::ostream & os) const
    {
        uint8_t r[] = {255, 0, 255,   0,   0, 255,   0, 255};
        uint8_t g[] = {255, 0,   0,   0, 255, 255, 255,   0};
        uint8_t b[] = {255, 0,   0, 255,   0,   0, 255, 255};
        os << "P6" << std::endl;
        os << m_w << ' ' << m_h << std::endl;
        os << 255 << std::endl;
        for (int i = 0; i < m_h; ++i)
        {
            for (int j = 0; j < m_w; ++j)
            {
                size_t p = static_cast<size_t>((*this)(i, j));
                os << r[p] << g[p] << b[p];
            }
        }
        return os;
    }

    bool valid(int i, int j) const
    {
        return (i >= 0) && (i < m_h) && (j >= 0) && (j < m_w);
    }

private:

    void resize(int w, int h)
    {
        m_w = w;
        m_h = h;
        m_data.resize(m_w * m_h);
    }

    int m_w, m_h;
    std::vector<cell_t> m_data;
    //std::vector<std::array<cell_index_t, 8>> m_neighbours;
};

#endif // MAP_HPP
