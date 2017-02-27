#pragma once

#include "types.hpp"
#include "Grid3D.hpp"
#include "threads.hpp"

#include "CubicalComplex3D.hpp"

namespace voxskel {

class ThinningProcessDGCI2013 {
public:
    // Initialize the thinning process of a cubical complex
    void init(CubicalComplex3D& cc,
              const Grid3D<uint32_t>* pDistanceMap,
              const Grid3D<uint32_t>* pOpeningMap,
              const CubicalComplex3D* pConstrainedCC = nullptr);

    // Apply the collapse operation to the cubical complex in each of the 6 directions
    //
    // \iterCount The number of collapse to apply. If -1, collapse till no free pair exists in the complex
    // \return true if the cubical complex ends up being thin.
    bool directionalCollapse(int iterCount = -1);

    // Same but using multiple threads
    bool parallelDirectionalCollapse(int iterCount = -1);

    const Grid3D<Vec3i>& getBirthMap() const {
        return m_BirthMap;
    }

public:
    using VoxelList = std::vector<Vec3i>;

    CubicalComplex3D* m_pCC = nullptr;
    const CubicalComplex3D* m_pConstrainedCC = nullptr;
    uint32_t m_nWidth = 0u;
    uint32_t m_nHeight = 0u;
    uint32_t m_nDepth = 0u;

    Grid3D<bool> m_BorderFlags; // Flag true for each voxel in the border
    VoxelList m_Borders[6]; // A list of voxels in the border for each direction-orientation of the 3D grid

    enum EdgeIndex {
        XEDGE_IDX = 0,
        YEDGE_IDX = 1,
        ZEDGE_IDX = 2,
    };

    Grid3D<Vec3i> m_BirthMap; // Contains the birth date of the edge of each voxel
    Grid3D<Vec3i> m_EdgeDistanceMap;
    Grid3D<Vec3i> m_EdgeOpeningMap;
    uint32_t m_nIterationCount = 0u; // Number of thinning iterations already done

    const Grid3D<uint32_t>* m_pDistanceMap; // Distance to border for each voxel
    const Grid3D<uint32_t>* m_pOpeningMap; // Opening radius for each voxel (based on the distance map)

    void collapseFreeFaces(int indexBorder,
                           int indexElement,
                           int direction,
                           int orientation,
                           int dimension);

    void testForBorder(int x, int y, int z);
    void clearBorder();

    void updateBorder();
    void computeUpdatedBorder(const VoxelList& updatedBorder);
    void clearBorderFlags(const VoxelList& updatedBorder);

    void updateBorderXNEG(VoxelList& updatedBorder);
    void updateBorderXPOS(VoxelList& updatedBorder);
    void updateBorderYNEG(VoxelList& updatedBorder);
    void updateBorderYPOS(VoxelList& updatedBorder);
    void updateBorderZNEG(VoxelList& updatedBorder);
    void updateBorderZPOS(VoxelList& updatedBorder);

    void updateBirthMap();

    bool isConstrainedEdge(int x, int y, int z, int edgeIdx) const;

    void setBorderFlag(int x, int y, int z, bool b = true) {
        m_BorderFlags(x, y, z) = b;
    }

    bool isInBorder(int x, int y, int z) const {
       return m_BorderFlags(x, y, z);
    }

    bool isFreeXNEG(int x, int y, int z) const {
        return isFree<CC3DDirection::X, CC3DOrientation::NEGATIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::NEGATIVE, CC3DFaceElement::XYFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::NEGATIVE, CC3DFaceElement::XZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::NEGATIVE, CC3DFaceElement::XEDGE>(Vec3i{ x, y, z });
    }

    bool isFreeXPOS(int x, int y, int z) const {
        return isFree<CC3DDirection::X, CC3DOrientation::POSITIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::POSITIVE, CC3DFaceElement::XYFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::POSITIVE, CC3DFaceElement::XZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::X, CC3DOrientation::POSITIVE, CC3DFaceElement::XEDGE>(Vec3i{ x, y, z });
    }

    bool isFreeYNEG(int x, int y, int z) const {
        return isFree<CC3DDirection::Y, CC3DOrientation::NEGATIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::NEGATIVE, CC3DFaceElement::YZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::NEGATIVE, CC3DFaceElement::XYFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::NEGATIVE, CC3DFaceElement::YEDGE>(Vec3i{ x, y, z });
    }

    bool isFreeYPOS(int x, int y, int z) const {
        return isFree<CC3DDirection::Y, CC3DOrientation::POSITIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::POSITIVE, CC3DFaceElement::YZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::POSITIVE, CC3DFaceElement::XYFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Y, CC3DOrientation::POSITIVE, CC3DFaceElement::YEDGE>(Vec3i{ x, y, z });
    }

    bool isFreeZNEG(int x, int y, int z) const {
        return isFree<CC3DDirection::Z, CC3DOrientation::NEGATIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::NEGATIVE, CC3DFaceElement::XZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::NEGATIVE, CC3DFaceElement::YZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::NEGATIVE, CC3DFaceElement::ZEDGE>(Vec3i{ x, y, z });
    }

    bool isFreeZPOS(int x, int y, int z) const {
        return isFree<CC3DDirection::Z, CC3DOrientation::POSITIVE, CC3DFaceElement::CUBE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::POSITIVE, CC3DFaceElement::XZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::POSITIVE, CC3DFaceElement::YZFACE>(Vec3i{ x, y, z }) ||
                isFree<CC3DDirection::Z, CC3DOrientation::POSITIVE, CC3DFaceElement::ZEDGE>(Vec3i{ x, y, z });
    }

    template<CC3DDirection Dir, CC3DOrientation Orient, CC3DFaceElement Face>
    struct EdgeConstraintHelper {
        static bool isEdgeConstrained(const ThinningProcessDGCI2013& thinningProcess,
                               const Vec3i& voxel) {
            bool result = false;
            CC3DFreePairHelper<Dir, Orient, Face>::forEach(voxel, [&](const Vec3i& v, int bits) {
                if((bits & CC3DFaceBits::XEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], XEDGE_IDX)) {
                    result = true;
                }
                if((bits & CC3DFaceBits::YEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], YEDGE_IDX)) {
                    result = true;
                }
                if((bits & CC3DFaceBits::ZEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], ZEDGE_IDX)) {
                    result = true;
                }
            });
            return result;
        }
    };

    template<CC3DDirection Dir, CC3DOrientation Orient>
    struct EdgeConstraintHelper<Dir, Orient, CC3DFaceElement::XEDGE> {
        static bool isEdgeConstrained(const ThinningProcessDGCI2013& thinningProcess,
                               const Vec3i& voxel) {
            bool result = false;
            CC3DFreePairHelper<Dir, Orient, CC3DFaceElement::XEDGE>::forEach(voxel, [&](const Vec3i& v, int bits) {
                if((bits & CC3DFaceBits::XEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], XEDGE_IDX)) {
                    result = true;
                }
            });
            return result;
        }
    };

    template<CC3DDirection Dir, CC3DOrientation Orient>
    struct EdgeConstraintHelper<Dir, Orient, CC3DFaceElement::YEDGE> {
        static bool isEdgeConstrained(const ThinningProcessDGCI2013& thinningProcess,
                               const Vec3i& voxel) {
            bool result = false;
            CC3DFreePairHelper<Dir, Orient, CC3DFaceElement::YEDGE>::forEach(voxel, [&](const Vec3i& v, int bits) {
                if((bits & CC3DFaceBits::YEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], YEDGE_IDX)) {
                    result = true;
                }
            });
            return result;
        }
    };

    template<CC3DDirection Dir, CC3DOrientation Orient>
    struct EdgeConstraintHelper<Dir, Orient, CC3DFaceElement::ZEDGE> {
        static bool isEdgeConstrained(const ThinningProcessDGCI2013& thinningProcess,
                               const Vec3i& voxel) {
            bool result = false;
            CC3DFreePairHelper<Dir, Orient, CC3DFaceElement::ZEDGE>::forEach(voxel, [&](const Vec3i& v, int bits) {
                if((bits & CC3DFaceBits::ZEDGE) && thinningProcess.isConstrainedEdge(v[0], v[1], v[2], ZEDGE_IDX)) {
                    result = true;
                }
            });
            return result;
        }
    };

    template<CC3DDirection Dir, CC3DOrientation Orient, CC3DFaceElement Face>
    bool isConstrained(const Vec3i& voxel) const {
        if(m_pConstrainedCC) {
            bool result = false;
            CC3DFreePairHelper<Dir, Orient, Face>::forEach(voxel, [&](const Vec3i& v, int bits) {
                result |= (*m_pConstrainedCC)(v).containsSome(bits);
            });
            if(result) {
                return true;
            }
        }
        if(!m_pDistanceMap || !m_pOpeningMap) {
            return false;
        }
        return EdgeConstraintHelper<Dir, Orient, Face>::isEdgeConstrained(*this, voxel);
    }

    template<CC3DDirection Dir, CC3DOrientation Orient, CC3DFaceElement Face>
    bool isFree(const Vec3i& voxel) const {
        return CC3DFreePairHelper<Dir, Orient, Face>::isFree(*m_pCC, voxel) && !isConstrained<Dir, Orient, Face>(voxel);
    }

    template<CC3DDirection Dir, CC3DOrientation Orient, CC3DFaceElement Face>
    void collapse(const Vec3i& voxel) {
        if(isFree<Dir, Orient, Face>(voxel)) {
            CC3DFreePairHelper<Dir, Orient, Face>::forEach(voxel, [&](const Vec3i& v, int bits) {
                (*m_pCC)(v).remove(bits);
            });
        }
    }

    template<typename Functor>
    inline void processBorder(const VoxelList& border, Functor f) {
        for(const auto& voxel: border) {
            f(voxel);
        }
        updateBorder();
    }

    template<typename Functor>
    inline void parallelProcessBorder(const VoxelList& border, Functor f) {
        processTasks(border.size(), [&](uint32_t taskID, uint32_t threadID) {
            f(border[taskID]);
        }, getSystemThreadCount());
    }

    bool borderIsEmpty() const {
        return m_Borders[0].empty() && m_Borders[1].empty() &&
               m_Borders[2].empty() && m_Borders[3].empty() &&
               m_Borders[4].empty() && m_Borders[5].empty();
    }

    size_t borderSize() const {
        return m_Borders[0].size() + m_Borders[1].size() +
               m_Borders[2].size() + m_Borders[3].size() +
               m_Borders[4].size() + m_Borders[5].size();
    }
};

}
