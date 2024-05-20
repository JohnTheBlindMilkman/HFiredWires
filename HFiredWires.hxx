/**
 * @file HFiredWires.hxx
 * @author Jędrzej Kołaś (jedrzej.kolas.dokt@pw.edu.pl)
 * @brief A utility class, which provides a simpler access to wires from which the track was reconstructed
 * @version 1.0
 * @date 2024-05-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HFiredWires_hxx
    #define HFiredWires_hxx

    #include <array>
    #include <vector>
    #include <iostream>

    #include "hades.h"
    #include "hcategory.h"
    #include "hcategorymanager.h"
    #include "hevent.h"
    #include "hmdcdef.h"
    #include "hmdcseg.h"
    #include "hparticlecand.h"
    #include "hparticlecandsim.h"
    
    namespace HADES
    {
        namespace MDC
        {
            // some global constants which might be helpful with the wire list
            namespace WireInfo
            {
                constexpr std::size_t numberOfInnerLayers = 12UL;
                constexpr std::size_t numberOfOuterLayers = 12UL;
                constexpr std::size_t numberOfLayersInPlane = 6UL;
                constexpr std::size_t numberOfPlanes = 4UL;
                constexpr std::size_t numberOfAllLayers = numberOfInnerLayers + numberOfOuterLayers;
                constexpr std::array<std::size_t,numberOfInnerLayers> halfLayerIndexing{0,1,2,3,4,5,6,7,8,9,10,11};
                constexpr std::array<std::size_t,numberOfAllLayers> allLayerIndexing{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
                constexpr std::array<std::array<std::size_t,numberOfLayersInPlane>,numberOfPlanes> allLayerPerPlaneIndexing
                    {
                        std::array<std::size_t,numberOfLayersInPlane>{0,1,2,3,4,5},
                        std::array<std::size_t,numberOfLayersInPlane>{6,7,8,9,10,11},
                        std::array<std::size_t,numberOfLayersInPlane>{12,13,14,15,16,17},
                        std::array<std::size_t,numberOfLayersInPlane>{18,19,20,21,22,23}
                    };
            }

            class HFiredWires
            {
                public:
                    /**
                     * @brief Get the collection of fired wires for the track specified in the constructor
                     * 
                     * @return std::array<std::vector<int>,WireInfo::numberOfAllLayers>
                     */
                    std::array<std::vector<int>,WireInfo::numberOfAllLayers> GetWires(HMdcSeg *inner, HMdcSeg *outer) const
                    {
                        std::array<std::vector<int>,WireInfo::numberOfAllLayers> FiredWireCollection;
                        int ncells = 0;

                        for (const int &layer : WireInfo::halfLayerIndexing)
                        {
                            ncells = inner->getNCells(layer);
                            if (ncells < 1)
                                continue;

                            for (int cell = 0; cell < ncells; ++cell)
                                FiredWireCollection.at(layer).push_back(inner->getCell(layer,cell));
                        }
                        for (const int &layer : WireInfo::halfLayerIndexing)
                        {
                            ncells = outer->getNCells(layer);
                            if (ncells < 1)
                                continue;

                            for (int cell = 0; cell < ncells; ++cell)
                                FiredWireCollection.at(layer+WireInfo::numberOfInnerLayers).push_back(outer->getCell(layer,cell));
                        }

                        return FiredWireCollection;
                    }
            };
        }
    }

#endif