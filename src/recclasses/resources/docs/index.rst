.. _recclasses:

Recclasses documentation
========================
This project is a self-contained collection of storage classes inserted into frames by modules other projects, including their pybindings and table converters. It is maintained by Hans Dembinski <hdembins@udel.edu>.

.. toctree::
   :maxdepth: 1

   parameter_storage
   laputop_params
   release_notes

Overview
--------
This project hosts a collection of storage classes, derived from I3FrameObject. These classes are filled by other projects (for example, I3LaputopParams is produced by project toprec). It was decided to put all these storage classes into this self-contained project with minimal dependencies, to offer convenient read/write access for consumers, who just want to do data analysis based on this additional information or who just want to browse events with Steamshovel. In that sense, recclasses is the analog of simclasses.   

A noteworthy class in this project is the :ref:`parameter_storage`, a general storage solution for users who want to store fitted parameters with full covariance matrix. Currently, it is used by :ref:`laputop_params`.

Most storage classes also have converter. To learn how to convert content from I3 files into table format, read :ref:`hdfwriter` or :ref:`rootwriter`.

.. _`hdfwriter`: http://software.icecube.wisc.edu/offline_trunk/projects/hdfwriter/
.. _`rootwriter`: http://software.icecube.wisc.edu/offline_trunk/projects/rootwriter/ 

Storage classes
---------------

Also see the full `doxygen <../../doxygen/recclasses/index.html>`_ docs.

==================================================================================================  ==================================================  ==================================================
storage classes                                                                                     converter                                           associated project                                
==================================================================================================  ==================================================  ==================================================
`I3CLastFitParams <../../doxygen/recclasses/classI3CLastFitParams.html>`_                           I3CLastFitParamsConverter                           :ref:`clast`                                      
`CramerRaoParams <../../doxygen/recclasses/classCramerRaoParams.html>`_                             CramerRaoParamsConverter                            :ref:`cramer-rao`                                 
`I3CscdLlhFitParams <../../doxygen/recclasses/classI3CscdLlhFitParams.html>`_                       I3CscdLlhFitParamsConverter                         :ref:`cscd-llh`                                   
`I3DST <../../doxygen/recclasses/classI3DST.html>`_                                                                                                     :ref:`dst-extractor`                              
`I3DST13 <../../doxygen/recclasses/classI3DST13.html>`_                                                                                                 :ref:`dst-extractor`                              
`I3DST16 <../../doxygen/recclasses/classI3DST16.html>`_                                                                                                 :ref:`dst-extractor`                              
`I3DSTHeader <../../doxygen/recclasses/classI3DSTHeader.html>`_                                                                                         :ref:`dst-extractor`                              
`I3DSTHeader13 <../../doxygen/recclasses/classI3DSTHeader13.html>`_                                                                                     :ref:`dst-extractor`                              
`I3DSTHeader16 <../../doxygen/recclasses/classI3DSTHeader16.html>`_                                                                                     :ref:`dst-extractor`                              
`I3DSTReco13 <../../doxygen/recclasses/classI3DSTReco13.html>`_                                                                                         :ref:`dst-extractor`                              
`I3DipoleFitParams <../../doxygen/recclasses/classI3DipoleFitParams.html>`_                         I3DipoleFitParamsConverter                          :ref:`dipolefit`                                  
`I3DirectHitsValues <../../doxygen/recclasses/classI3DirectHitsValues.html>`_                       I3DirectHitsValuesConverter (Python)                :ref:`CommonVariables`                            
`I3FillRatioInfo <../../doxygen/recclasses/classI3FillRatioInfo.html>`_                             I3FillRatioInfoConverter                            :ref:`fill-ratio`                                 
`I3FiniteCuts <../../doxygen/recclasses/classI3FiniteCuts.html>`_                                                                                       :ref:`finiteReco`                                 
`I3HitMultiplicityValues <../../doxygen/recclasses/classI3HitMultiplicityValues.html>`_             I3HitMultiplicityValuesConverter (Python)           :ref:`CommonVariables`                            
`I3HitStatisticsValues <../../doxygen/recclasses/classI3HitStatisticsValues.html>`_                 I3HitStatisticsValuesConverter (Python)             :ref:`CommonVariables`                            
`I3LaputopParams <../../doxygen/recclasses/classI3LaputopParams.html>`_                             I3LaputopParamsConverter                            :ref:`toprec`                                     
`I3LineFitParams <../../doxygen/recclasses/classI3LineFitParams.html>`_                             I3LineFitParamsConverter                                                                              
`OMKeyLink <../../doxygen/recclasses/classOMKeyLink.html>`_                                                                                                                                               
`OMKeyLinkSet <../../doxygen/recclasses/classOMKeyLinkSet.html>`_                                                                                       :ref:`CommonVariables`                            
`OMKeyPair <../../doxygen/recclasses/classOMKeyPair.html>`_                                                                                                                                               
`OMKeySet <../../doxygen/recclasses/classOMKeySet.html>`_                                                                                                                                                 
`I3OpheliaFirstGuessTrack <../../doxygen/recclasses/classI3OpheliaFirstGuessTrack.html>`_           I3OpheliaFirstGuessTrackConverter                   :ref:`ophelia`                                    
`I3OpheliaParticle <../../doxygen/recclasses/classI3OpheliaParticle.html>`_                                                                             :ref:`ophelia`                                    
`I3OpheliaRecoResult <../../doxygen/recclasses/classI3OpheliaRecoResult.html>`_                                                                         :ref:`ophelia`                                    
`I3ParticleIntersections <../../doxygen/recclasses/classI3ParticleIntersections.html>`_                                                                 :ref:`VHESelfVeto`                                
`I3PortiaEvent <../../doxygen/recclasses/classI3PortiaEvent.html>`_                                 I3PortiaEventConverter                              :ref:`portia`                                     
`I3PortiaPulse <../../doxygen/recclasses/classI3PortiaPulse.html>`_                                                                                     :ref:`portia`                                     
`I3STConfiguration <../../doxygen/recclasses/classI3STConfiguration.html>`_                                                                             :ref:`STTools`                                    
`I3ShieldHitRecord <../../doxygen/recclasses/classI3ShieldHitRecord.html>`_                         I3ShieldHitRecordConverter                                                                            
`I3StartStopParams <../../doxygen/recclasses/classI3StartStopParams.html>`_                                                                             :ref:`finiteReco`                                 
`I3TensorOfInertiaFitParams <../../doxygen/recclasses/classI3TensorOfInertiaFitParams.html>`_       I3TensorOfInertiaFitParamsConverter                 :ref:`tensor-of-inertia`                          
`I3TimeCharacteristicsValues <../../doxygen/recclasses/classI3TimeCharacteristicsValues.html>`_     I3TimeCharacteristicsValuesConverter (Python)       :ref:`CommonVariables`                            
`I3TopLateralFitParams <../../doxygen/recclasses/classI3TopLateralFitParams.html>`_                 I3TopLateralFitParamsConverter                      :ref:`toprec`                                     
`I3TopRecoPlaneFitParams <../../doxygen/recclasses/classI3TopRecoPlaneFitParams.html>`_             I3TopRecoPlaneFitParamsConverter                    :ref:`toprec`                                     
`I3TrackCharacteristicsValues <../../doxygen/recclasses/classI3TrackCharacteristicsValues.html>`_   I3TrackCharacteristicsValuesConverter (Python)      :ref:`CommonVariables`                            
`I3Veto <../../doxygen/recclasses/classI3Veto.html>`_                                               I3VetoConverter (Python)                            :ref:`CascadeVariables`                           
`I3VetoShort <../../doxygen/recclasses/classI3VetoShort.html>`_                                                                                         :ref:`CascadeVariables`                           
==================================================================================================  ==================================================  ==================================================
