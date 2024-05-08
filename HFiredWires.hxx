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
            // some global constants whoch might be helpful with the wire list
            namespace WireInfo
            {
                constexpr std::size_t numberOfInnerLayers = 12UL;
                constexpr std::size_t numberOfOuterLayers = 12UL;
                constexpr std::size_t numberOfAllLayers = numberOfInnerLayers + numberOfOuterLayers;
                constexpr std::array<std::size_t,numberOfInnerLayers> halfLayerIndexing{0,1,2,3,4,5,6,7,8,9,10,11};
                constexpr std::array<std::size_t,numberOfAllLayers> allLayerIndexing{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
            }

            class HFiredWires
            {
                private:
                    std::array<std::vector<int>,WireInfo::numberOfAllLayers> FiredWireCollection;
                    HCategory *mdc_seg_cat;
                    HMdcSeg *innerSeg,*outerSeg;
                    bool MdcCatIsNotSet;

                    /**
                     * @brief Set the Mdc Seg Category object
                     * 
                     * @throw runtime_error if category is not found
                     */
                    void SetMdcSegCategory()
                    {
                        mdc_seg_cat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcSeg);
                        if (mdc_seg_cat == nullptr)
                            throw std::runtime_error("HFiredWires::SetMdcSegCategory() catMdcSeg not available");

                        MdcCatIsNotSet = false;
                    }
                    /**
                     * @brief Set the Inner And Outer Segments object when runing DST
                     * 
                     * @param cand 
                     * @throw runtime_error if the segments are not found
                     */
                    void SetInnerAndOuterSegments(HParticleCand *cand)
                    {
                        innerSeg = HCategoryManager::getObject(innerSeg,mdc_seg_cat,cand->getInnerSegInd());
                        outerSeg = HCategoryManager::getObject(outerSeg,mdc_seg_cat,cand->getOuterSegInd());

                        if (innerSeg == nullptr || outerSeg == nullptr)
                            throw std::runtime_error("HFiredWires::SetInnerAndOuterSegments() inner and/or outer mdc segment is null");
                    }
                    /**
                     * @brief Set the Inner And Outer Segments object when runing HGeant
                     * 
                     * @param cand 
                     * @throw runtime_error if the segments are not found
                     */
                    void SetInnerAndOuterSegments(HParticleCandSim *cand)
                    {
                        innerSeg = HCategoryManager::getObject(innerSeg,mdc_seg_cat,cand->getInnerSegInd());
                        outerSeg = HCategoryManager::getObject(outerSeg,mdc_seg_cat,cand->getOuterSegInd());

                        if (innerSeg == nullptr || outerSeg == nullptr)
                            throw std::runtime_error("HFiredWires::SetInnerAndOuterSegments() inner and/or outer mdc segment is null");
                    }
                    /**
                     * @brief Map wires from HMdcSeg to the std::array<std::vector<int>,24>
                     * 
                     */
                    void SetWires()
                    {
                        int ncells = 0;

                        for (const int &layer : WireInfo::halfLayerIndexing)
                        {
                            ncells = innerSeg->getNCells(layer);
                            if (ncells < 1)
                                continue;

                            for (int cell = 0; cell < ncells; ++cell)
                                FiredWireCollection.at(layer).push_back(innerSeg->getCell(layer,cell));
                        }
                        for (const int &layer : WireInfo::halfLayerIndexing)
                        {
                            ncells = outerSeg->getNCells(layer);
                            if (ncells < 1)
                                continue;

                            for (int cell = 0; cell < ncells; ++cell)
                                FiredWireCollection.at(layer+WireInfo::numberOfInnerLayers).push_back(outerSeg->getCell(layer,cell));
                        }
                    }

                public:
                    /**
                     * @brief Construct a new HFiredWires object. Construct the object once, before the event loop
                     * 
                     */
                    HFiredWires() : MdcCatIsNotSet(true)
                    {
                        SetMdcSegCategory();
                    }
                    /**
                     * @brief Construct a new HFiredWires object for this DST track. Invoke this method every new track, before trying to access the wires collection
                     * 
                     * @param cand 
                     */
                    HFiredWires(HParticleCand *cand)
                    {
                        if (MdcCatIsNotSet)
                            SetMdcSegCategory();
                        SetInnerAndOuterSegments(cand);
                        SetWires();
                    }
                    /**
                     * @brief Construct a new HFiredWires object for this HGeant track. Invoke this method every new track, before trying to access the wires collection
                     * 
                     * @param cand 
                     */
                    HFiredWires(HParticleCandSim *cand)
                    {
                        if (MdcCatIsNotSet)
                            SetMdcSegCategory();
                        SetInnerAndOuterSegments(cand);
                        SetWires();
                    }
                    /**
                     * @brief Get the collection of fired wires for the track specified in the constructor
                     * 
                     * @return std::array<std::vector<int>,WireInfo::numberOfAllLayers>
                     */
                    std::array<std::vector<int>,WireInfo::numberOfAllLayers> GetWires() const
                    {
                        return FiredWireCollection;
                    }
                    /**
                     * @brief Prints to the standard output all the indexes of the wires that were hit
                     * 
                     */
                    void PrintWires() const
                    {
                        for (const std::size_t &layer : WireInfo::allLayerIndexing)
                        {
                            if (layer == 0)
                                std::cout << "\n\n---=== Inner Segment ===---\n";
                            else if (layer == 12)
                                std::cout << "---=== Outer Segment ===---\n";

                            std::cout << "Layer " << layer+1 << ":\t";
                            for (const auto &wire : FiredWireCollection.at(layer))
                                std::cout << wire << "\t";

                            std::cout << "\n";
                        }
                        std::cout << "\n\n";
                    }
            };
        }
    }

#endif