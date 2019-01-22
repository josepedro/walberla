//======================================================================================================================
//
//  This file is part of waLBerla. waLBerla is free software: you can
//  redistribute it and/or modify it under the terms of the GNU General Public
//  License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version.
//
//  waLBerla is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with waLBerla (see COPYING.txt). If not, see <http://www.gnu.org/licenses/>.
//
//! \\author Martin Bauer <martin.bauer@fau.de>
//
//======================================================================================================================


#include "core/DataTypes.h"
#include "core/logging/Logging.h"

#include "field/GhostLayerField.h"
#include "field/SwapableCompare.h"
#include "domain_decomposition/BlockDataID.h"
#include "domain_decomposition/IBlock.h"
#include "stencil/D3Q19.h"

#include "lbm/lattice_model/EquilibriumDistribution.h"
#include "lbm/field/Density.h"
#include "lbm/field/DensityAndMomentumDensity.h"
#include "lbm/field/DensityAndVelocity.h"
#include "lbm/field/PressureTensor.h"
#include "lbm/field/ShearRate.h"

#include <set>

#ifdef __GNUC__
#define RESTRICT __restrict__
#elif _MSC_VER
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

#ifdef WALBERLA_CXX_COMPILER_IS_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ifdef WALBERLA_CXX_COMPILER_IS_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif




// Forward declarations
namespace walberla{
namespace lbm {
   class UniformGridGPU_LatticeModel;
}}
namespace walberla {
namespace mpi {
    mpi::SendBuffer & operator<< (mpi::SendBuffer & buf, const ::walberla::lbm::UniformGridGPU_LatticeModel & lm);
    mpi::RecvBuffer & operator>> (mpi::RecvBuffer & buf,       ::walberla::lbm::UniformGridGPU_LatticeModel & lm);
}}




namespace walberla {
namespace lbm {


/**
UniformGridGPU_LatticeModel was generated with lbmpy. Do not edit this file directly. Instead modify UniformGridGPU_LatticeModel.py.
For details see documentation of lbmpy.

Usage:
    - Create an instance of this lattice model class: the constructor parameters vary depending on the configure
      lattice model. A model with constant force needs a single force vector, while a model with variable forces needs
      a force field. All constructor parameters are ordered alphabetically.
    - Create a PDFField with the lattice model as template argument to store the particle distribution functions.
      Use the PDFField to get and modify macroscopic values.
    - The internal class UniformGridGPU_LatticeModel::Sweep is a functor to execute one LB time step.
      Stream, collide steps can be executed separately, or together in an optimized stream-pull-collide scheme

*/
class UniformGridGPU_LatticeModel
{

public:
    typedef stencil::D3Q19 Stencil;
    typedef stencil::D3Q19 CommunicationStencil;
    static const real_t w[19];
    static const real_t wInv[19];

    static const bool compressible = false;
    static const int equilibriumAccuracyOrder = 2;

    class Sweep
    {
    public:
        Sweep( BlockDataID _pdfsID ) : pdfsID(_pdfsID) {};

        //void stream       ( IBlock * const block, const uint_t numberOfGhostLayersToInclude = uint_t(0) );
        void collide      ( IBlock * const block, const uint_t numberOfGhostLayersToInclude = uint_t(0) );
        void streamCollide( IBlock * const block, const uint_t numberOfGhostLayersToInclude = uint_t(0) );
        void stream       ( IBlock * const block, const uint_t numberOfGhostLayersToInclude = uint_t(0) );

        void operator() ( IBlock * const block, const uint_t numberOfGhostLayersToInclude = uint_t(0) )
        {
            streamCollide( block, numberOfGhostLayersToInclude );
        }

    private:
        BlockDataID pdfsID;

        std::set< GhostLayerField<double, 19> *, field::SwapableCompare< GhostLayerField<double, 19> * > > cache_pdfs_;
    };

    UniformGridGPU_LatticeModel( double omega_ )
        : omega(omega_), currentLevel(0)
    {};

    void configure( IBlock & block, StructuredBlockStorage &)  { configureBlock( &block ); }

private:
    void configureBlock(IBlock * block)
    {
        


        }

    // Parameters:
    double omega;

    // Updated by configureBlock:
    

    uint_t currentLevel;

    // Backend classes can access private members:
    friend class UniformGridGPU_LatticeModel::Sweep;
    template<class LM, class Enable> friend class  EquilibriumDistribution;
    template<class LM, class Enable> friend struct Equilibrium;
    template<class LM, class Enable> friend struct internal::AdaptVelocityToForce;
    template<class LM, class Enable> friend struct Density;
    template<class LM>               friend struct DensityAndVelocity;
    template<class LM, class Enable> friend struct DensityAndMomentumDensity;
    template<class LM, class Enable> friend struct MomentumDensity;
    template<class LM, class It, class Enable> friend struct DensityAndVelocityRange;

    friend mpi::SendBuffer & ::walberla::mpi::operator<< (mpi::SendBuffer & , const UniformGridGPU_LatticeModel & );
    friend mpi::RecvBuffer & ::walberla::mpi::operator>> (mpi::RecvBuffer & ,       UniformGridGPU_LatticeModel & );

};




//======================================================================================================================
//
//  Implementation of macroscopic value backend
//
//======================================================================================================================



template<>
class EquilibriumDistribution< UniformGridGPU_LatticeModel, void>
{
public:
   typedef typename UniformGridGPU_LatticeModel::Stencil Stencil;

   static real_t get( const stencil::Direction direction,
                      const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ),
                      real_t rho = real_t(1.0) )
   {
        
        rho -= real_t(1.0);
        
        
    using namespace stencil;
    switch( direction ) {
        case C: return rho*0.333333333333333 - 0.333333333333333*(u[0]*u[0]) - 0.333333333333333*(u[1]*u[1]) - 0.333333333333333*(u[2]*u[2]);
        case N: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*u[1];
        case S: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*u[1] - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]);
        case W: return rho*0.0555555555555556 - 0.166666666666667*u[0] - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]);
        case E: return rho*0.0555555555555556 - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]) + 0.166666666666667*u[0];
        case T: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*u[2];
        case B: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*u[2] + 0.166666666666667*(u[2]*u[2]);
        case NW: return rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1];
        case NE: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.25*u[0]*u[1];
        case SW: return rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.25*u[0]*u[1];
        case SE: return rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]);
        case TN: return rho*0.0277777777777778 + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[1]*u[2];
        case TS: return rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
        case TW: return rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
        case TE: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[0]*u[2];
        case BN: return rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]);
        case BS: return rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[1]*u[2];
        case BW: return rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[0]*u[2];
        case BE: return rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]);
        default:
            WALBERLA_ABORT("Invalid Direction");
    }
    
   }

   static real_t getSymmetricPart( const stencil::Direction direction,
                                   const Vector3<real_t> & u = Vector3< real_t >(real_t(0.0)),
                                   real_t rho = real_t(1.0) )
   {
        
        rho -= real_t(1.0);
        
        
    using namespace stencil;
    switch( direction ) {
        case C: return rho*0.333333333333333 - 0.333333333333333*(u[0]*u[0]) - 0.333333333333333*(u[1]*u[1]) - 0.333333333333333*(u[2]*u[2]);
        case N: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]);
        case S: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]);
        case W: return rho*0.0555555555555556 - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]);
        case E: return rho*0.0555555555555556 - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]);
        case T: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*(u[2]*u[2]);
        case B: return rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*(u[2]*u[2]);
        case NW: return rho*0.0277777777777778 - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]);
        case NE: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.25*u[0]*u[1];
        case SW: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.25*u[0]*u[1];
        case SE: return rho*0.0277777777777778 - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]);
        case TN: return rho*0.0277777777777778 + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[1]*u[2];
        case TS: return rho*0.0277777777777778 - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]);
        case TW: return rho*0.0277777777777778 - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]);
        case TE: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[0]*u[2];
        case BN: return rho*0.0277777777777778 - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]);
        case BS: return rho*0.0277777777777778 + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[1]*u[2];
        case BW: return rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[0]*u[2];
        case BE: return rho*0.0277777777777778 - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]);
        default:
            WALBERLA_ABORT("Invalid Direction");
    }
    
   }

   static real_t getAsymmetricPart( const stencil::Direction direction,
                                    const Vector3< real_t > & u = Vector3<real_t>( real_t(0.0) ),
                                    real_t rho = real_t(1.0) )
   {
        
        rho -= real_t(1.0);
        
        
    using namespace stencil;
    switch( direction ) {
        case C: return 0;
        case N: return 0.166666666666667*u[1];
        case S: return -0.166666666666667*u[1];
        case W: return -0.166666666666667*u[0];
        case E: return 0.166666666666667*u[0];
        case T: return 0.166666666666667*u[2];
        case B: return -0.166666666666667*u[2];
        case NW: return -0.0833333333333333*u[0] + 0.0833333333333333*u[1];
        case NE: return 0.0833333333333333*u[0] + 0.0833333333333333*u[1];
        case SW: return -0.0833333333333333*u[0] - 0.0833333333333333*u[1];
        case SE: return -0.0833333333333333*u[1] + 0.0833333333333333*u[0];
        case TN: return 0.0833333333333333*u[1] + 0.0833333333333333*u[2];
        case TS: return -0.0833333333333333*u[1] + 0.0833333333333333*u[2];
        case TW: return -0.0833333333333333*u[0] + 0.0833333333333333*u[2];
        case TE: return 0.0833333333333333*u[0] + 0.0833333333333333*u[2];
        case BN: return -0.0833333333333333*u[2] + 0.0833333333333333*u[1];
        case BS: return -0.0833333333333333*u[1] - 0.0833333333333333*u[2];
        case BW: return -0.0833333333333333*u[0] - 0.0833333333333333*u[2];
        case BE: return -0.0833333333333333*u[2] + 0.0833333333333333*u[0];
        default:
            WALBERLA_ABORT("Invalid Direction");
    }
    
   }

   static std::vector< real_t > get( const Vector3< real_t > & u = Vector3<real_t>( real_t(0.0) ),
                                     real_t rho = real_t(1.0) )
   {
      
      rho -= real_t(1.0);
      

      std::vector< real_t > equilibrium( Stencil::Size );
      for( auto d = Stencil::begin(); d != Stencil::end(); ++d )
      {
         equilibrium[d.toIdx()] = get(*d, u, rho);
      }
      return equilibrium;
   }
};


namespace internal {

template<>
struct AdaptVelocityToForce<UniformGridGPU_LatticeModel, void>
{
   template< typename FieldPtrOrIterator >
   static Vector3<real_t> get( FieldPtrOrIterator & it, const UniformGridGPU_LatticeModel & lm,
                               const Vector3< real_t > & velocity, const real_t rho )
   {
      auto x = it.x();
      auto y = it.y();
      auto z = it.z();
      
      return velocity;
      
   }

   static Vector3<real_t> get( const cell_idx_t x, const cell_idx_t y, const cell_idx_t z, const UniformGridGPU_LatticeModel & lm,
                               const Vector3< real_t > & velocity, const real_t rho )
   {
      
      return velocity;
      
   }
};
} // namespace internal



template<>
struct Equilibrium< UniformGridGPU_LatticeModel, void >
{

   template< typename FieldPtrOrIterator >
   static void set( FieldPtrOrIterator & it,
                    const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ), real_t rho = real_t(1.0) )
   {
        
        rho -= real_t(1.0);
        

       it[0] = rho*0.333333333333333 - 0.333333333333333*(u[0]*u[0]) - 0.333333333333333*(u[1]*u[1]) - 0.333333333333333*(u[2]*u[2]);
       it[1] = rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*u[1];
       it[2] = rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*u[1] - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]);
       it[3] = rho*0.0555555555555556 - 0.166666666666667*u[0] - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]);
       it[4] = rho*0.0555555555555556 - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]) + 0.166666666666667*u[0];
       it[5] = rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*u[2];
       it[6] = rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*u[2] + 0.166666666666667*(u[2]*u[2]);
       it[7] = rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1];
       it[8] = rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.25*u[0]*u[1];
       it[9] = rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.25*u[0]*u[1];
       it[10] = rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]);
       it[11] = rho*0.0277777777777778 + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[1]*u[2];
       it[12] = rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
       it[13] = rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
       it[14] = rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[0]*u[2];
       it[15] = rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]);
       it[16] = rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[1]*u[2];
       it[17] = rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[0]*u[2];
       it[18] = rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]);
       }

   template< typename PdfField_T >
   static void set( PdfField_T & pdf, const cell_idx_t x, const cell_idx_t y, const cell_idx_t z,
                    const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ), real_t rho = real_t(1.0) )
   {
      
      rho -= real_t(1.0);
      

      real_t & xyz0 = pdf(x,y,z,0);
      pdf.getF( &xyz0, 0)= rho*0.333333333333333 - 0.333333333333333*(u[0]*u[0]) - 0.333333333333333*(u[1]*u[1]) - 0.333333333333333*(u[2]*u[2]);
      pdf.getF( &xyz0, 1)= rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*u[1];
      pdf.getF( &xyz0, 2)= rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*u[1] - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[1]*u[1]);
      pdf.getF( &xyz0, 3)= rho*0.0555555555555556 - 0.166666666666667*u[0] - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]);
      pdf.getF( &xyz0, 4)= rho*0.0555555555555556 - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*(u[0]*u[0]) + 0.166666666666667*u[0];
      pdf.getF( &xyz0, 5)= rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) + 0.166666666666667*(u[2]*u[2]) + 0.166666666666667*u[2];
      pdf.getF( &xyz0, 6)= rho*0.0555555555555556 - 0.166666666666667*(u[0]*u[0]) - 0.166666666666667*(u[1]*u[1]) - 0.166666666666667*u[2] + 0.166666666666667*(u[2]*u[2]);
      pdf.getF( &xyz0, 7)= rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1];
      pdf.getF( &xyz0, 8)= rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.25*u[0]*u[1];
      pdf.getF( &xyz0, 9)= rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[1]*u[1]) + 0.25*u[0]*u[1];
      pdf.getF( &xyz0, 10)= rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[0]*u[1] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[1]*u[1]);
      pdf.getF( &xyz0, 11)= rho*0.0277777777777778 + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[1]*u[2];
      pdf.getF( &xyz0, 12)= rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
      pdf.getF( &xyz0, 13)= rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2];
      pdf.getF( &xyz0, 14)= rho*0.0277777777777778 + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]) + 0.0833333333333333*u[2] + 0.25*u[0]*u[2];
      pdf.getF( &xyz0, 15)= rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[1]*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*u[1] + 0.0833333333333333*(u[2]*u[2]);
      pdf.getF( &xyz0, 16)= rho*0.0277777777777778 - 0.0833333333333333*u[1] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[1]*u[1]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[1]*u[2];
      pdf.getF( &xyz0, 17)= rho*0.0277777777777778 - 0.0833333333333333*u[0] - 0.0833333333333333*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*(u[2]*u[2]) + 0.25*u[0]*u[2];
      pdf.getF( &xyz0, 18)= rho*0.0277777777777778 - 0.0833333333333333*u[2] - 0.25*u[0]*u[2] + 0.0833333333333333*(u[0]*u[0]) + 0.0833333333333333*u[0] + 0.0833333333333333*(u[2]*u[2]);
      }
};


template<>
struct Density<UniformGridGPU_LatticeModel, void>
{
   template< typename FieldPtrOrIterator >
   static inline real_t get( const UniformGridGPU_LatticeModel & , const FieldPtrOrIterator & it )
   {
        const real_t f_0 = it[0];
        const real_t f_1 = it[1];
        const real_t f_2 = it[2];
        const real_t f_3 = it[3];
        const real_t f_4 = it[4];
        const real_t f_5 = it[5];
        const real_t f_6 = it[6];
        const real_t f_7 = it[7];
        const real_t f_8 = it[8];
        const real_t f_9 = it[9];
        const real_t f_10 = it[10];
        const real_t f_11 = it[11];
        const real_t f_12 = it[12];
        const real_t f_13 = it[13];
        const real_t f_14 = it[14];
        const real_t f_15 = it[15];
        const real_t f_16 = it[16];
        const real_t f_17 = it[17];
        const real_t f_18 = it[18];
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        return rho;
   }

   template< typename PdfField_T >
   static inline real_t get( const UniformGridGPU_LatticeModel & ,
                             const PdfField_T & pdf, const cell_idx_t x, const cell_idx_t y, const cell_idx_t z )
   {
        const real_t & xyz0 = pdf(x,y,z,0);
        const real_t f_0 = pdf.getF( &xyz0, 0);
        const real_t f_1 = pdf.getF( &xyz0, 1);
        const real_t f_2 = pdf.getF( &xyz0, 2);
        const real_t f_3 = pdf.getF( &xyz0, 3);
        const real_t f_4 = pdf.getF( &xyz0, 4);
        const real_t f_5 = pdf.getF( &xyz0, 5);
        const real_t f_6 = pdf.getF( &xyz0, 6);
        const real_t f_7 = pdf.getF( &xyz0, 7);
        const real_t f_8 = pdf.getF( &xyz0, 8);
        const real_t f_9 = pdf.getF( &xyz0, 9);
        const real_t f_10 = pdf.getF( &xyz0, 10);
        const real_t f_11 = pdf.getF( &xyz0, 11);
        const real_t f_12 = pdf.getF( &xyz0, 12);
        const real_t f_13 = pdf.getF( &xyz0, 13);
        const real_t f_14 = pdf.getF( &xyz0, 14);
        const real_t f_15 = pdf.getF( &xyz0, 15);
        const real_t f_16 = pdf.getF( &xyz0, 16);
        const real_t f_17 = pdf.getF( &xyz0, 17);
        const real_t f_18 = pdf.getF( &xyz0, 18);
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        return rho;
   }
};


template<>
struct DensityAndVelocity<UniformGridGPU_LatticeModel>
{
    template< typename FieldPtrOrIterator >
    static void set( FieldPtrOrIterator & it, const UniformGridGPU_LatticeModel & lm,
                     const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ), const real_t rho_in = real_t(1.0) )
    {
        auto x = it.x();
        auto y = it.y();
        auto z = it.z();

        const double rho = rho_in - 1;
        const double u_0 = u[0];
        const double u_1 = u[1];
        const double u_2 = u[2];
        

        Equilibrium<UniformGridGPU_LatticeModel>::set(it, Vector3<real_t>(u_0, u_1, u_2), rho + real_t(1) );
    }

    template< typename PdfField_T >
    static void set( PdfField_T & pdf, const cell_idx_t x, const cell_idx_t y, const cell_idx_t z, const UniformGridGPU_LatticeModel & lm,
                     const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ), const real_t rho_in = real_t(1.0) )
    {
        const double rho = rho_in - 1;
        const double u_0 = u[0];
        const double u_1 = u[1];
        const double u_2 = u[2];
        

        Equilibrium<UniformGridGPU_LatticeModel>::set(pdf, x, y, z, Vector3<real_t>(u_0, u_1, u_2), rho  + real_t(1) );
    }
};


template<typename FieldIteratorXYZ >
struct DensityAndVelocityRange<UniformGridGPU_LatticeModel, FieldIteratorXYZ>
{

   static void set( FieldIteratorXYZ & begin, const FieldIteratorXYZ & end, const UniformGridGPU_LatticeModel & lm,
                    const Vector3< real_t > & u = Vector3< real_t >( real_t(0.0) ), const real_t rho_in = real_t(1.0) )
   {
        for( auto cellIt = begin; cellIt != end; ++cellIt )
        {
            const auto x = cellIt.x();
            const auto y = cellIt.y();
            const auto z = cellIt.z();
            const double rho = rho_in - 1;
            const double u_0 = u[0];
            const double u_1 = u[1];
            const double u_2 = u[2];
            

            Equilibrium<UniformGridGPU_LatticeModel>::set(cellIt, Vector3<real_t>(u_0, u_1, u_2), rho + real_t(1) );
        }
   }
};



template<>
struct DensityAndMomentumDensity<UniformGridGPU_LatticeModel>
{
   template< typename FieldPtrOrIterator >
   static real_t get( Vector3< real_t > & momentumDensity, const UniformGridGPU_LatticeModel & lm,
                      const FieldPtrOrIterator & it )
   {
        const auto x = it.x();
        const auto y = it.y();
        const auto z = it.z();

        const real_t f_0 = it[0];
        const real_t f_1 = it[1];
        const real_t f_2 = it[2];
        const real_t f_3 = it[3];
        const real_t f_4 = it[4];
        const real_t f_5 = it[5];
        const real_t f_6 = it[6];
        const real_t f_7 = it[7];
        const real_t f_8 = it[8];
        const real_t f_9 = it[9];
        const real_t f_10 = it[10];
        const real_t f_11 = it[11];
        const real_t f_12 = it[12];
        const real_t f_13 = it[13];
        const real_t f_14 = it[14];
        const real_t f_15 = it[15];
        const real_t f_16 = it[16];
        const real_t f_17 = it[17];
        const real_t f_18 = it[18];
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        const double md_0 = -f_13 - f_17 - f_3 - f_7 - f_9 + vel0Term;
        const double md_1 = -f_10 - f_12 - f_16 - f_2 + f_8 - f_9 + vel1Term;
        const double md_2 = f_11 + f_14 - f_15 - f_16 - f_17 - f_18 - f_6 + vel2Term;
        momentumDensity[0] = md_0;
        momentumDensity[1] = md_1;
        momentumDensity[2] = md_2;
        
        return rho;
   }

   template< typename PdfField_T >
   static real_t get( Vector3< real_t > & momentumDensity, const UniformGridGPU_LatticeModel & lm, const PdfField_T & pdf,
                      const cell_idx_t x, const cell_idx_t y, const cell_idx_t z )
   {
        const real_t & xyz0 = pdf(x,y,z,0);
        const real_t f_0 = pdf.getF( &xyz0, 0);
        const real_t f_1 = pdf.getF( &xyz0, 1);
        const real_t f_2 = pdf.getF( &xyz0, 2);
        const real_t f_3 = pdf.getF( &xyz0, 3);
        const real_t f_4 = pdf.getF( &xyz0, 4);
        const real_t f_5 = pdf.getF( &xyz0, 5);
        const real_t f_6 = pdf.getF( &xyz0, 6);
        const real_t f_7 = pdf.getF( &xyz0, 7);
        const real_t f_8 = pdf.getF( &xyz0, 8);
        const real_t f_9 = pdf.getF( &xyz0, 9);
        const real_t f_10 = pdf.getF( &xyz0, 10);
        const real_t f_11 = pdf.getF( &xyz0, 11);
        const real_t f_12 = pdf.getF( &xyz0, 12);
        const real_t f_13 = pdf.getF( &xyz0, 13);
        const real_t f_14 = pdf.getF( &xyz0, 14);
        const real_t f_15 = pdf.getF( &xyz0, 15);
        const real_t f_16 = pdf.getF( &xyz0, 16);
        const real_t f_17 = pdf.getF( &xyz0, 17);
        const real_t f_18 = pdf.getF( &xyz0, 18);
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        const double md_0 = -f_13 - f_17 - f_3 - f_7 - f_9 + vel0Term;
        const double md_1 = -f_10 - f_12 - f_16 - f_2 + f_8 - f_9 + vel1Term;
        const double md_2 = f_11 + f_14 - f_15 - f_16 - f_17 - f_18 - f_6 + vel2Term;
        momentumDensity[0] = md_0;
        momentumDensity[1] = md_1;
        momentumDensity[2] = md_2;
        
       return rho;
   }
};


template<>
struct MomentumDensity< UniformGridGPU_LatticeModel>
{
   template< typename FieldPtrOrIterator >
   static void get( Vector3< real_t > & momentumDensity, const UniformGridGPU_LatticeModel & lm, const FieldPtrOrIterator & it )
   {
        const auto x = it.x();
        const auto y = it.y();
        const auto z = it.z();

        const real_t f_0 = it[0];
        const real_t f_1 = it[1];
        const real_t f_2 = it[2];
        const real_t f_3 = it[3];
        const real_t f_4 = it[4];
        const real_t f_5 = it[5];
        const real_t f_6 = it[6];
        const real_t f_7 = it[7];
        const real_t f_8 = it[8];
        const real_t f_9 = it[9];
        const real_t f_10 = it[10];
        const real_t f_11 = it[11];
        const real_t f_12 = it[12];
        const real_t f_13 = it[13];
        const real_t f_14 = it[14];
        const real_t f_15 = it[15];
        const real_t f_16 = it[16];
        const real_t f_17 = it[17];
        const real_t f_18 = it[18];
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        const double md_0 = -f_13 - f_17 - f_3 - f_7 - f_9 + vel0Term;
        const double md_1 = -f_10 - f_12 - f_16 - f_2 + f_8 - f_9 + vel1Term;
        const double md_2 = f_11 + f_14 - f_15 - f_16 - f_17 - f_18 - f_6 + vel2Term;
        momentumDensity[0] = md_0;
        momentumDensity[1] = md_1;
        momentumDensity[2] = md_2;
        
   }

   template< typename PdfField_T >
   static void get( Vector3< real_t > & momentumDensity, const UniformGridGPU_LatticeModel & lm, const PdfField_T & pdf,
                    const cell_idx_t x, const cell_idx_t y, const cell_idx_t z )
   {
        const real_t & xyz0 = pdf(x,y,z,0);
        const real_t f_0 = pdf.getF( &xyz0, 0);
        const real_t f_1 = pdf.getF( &xyz0, 1);
        const real_t f_2 = pdf.getF( &xyz0, 2);
        const real_t f_3 = pdf.getF( &xyz0, 3);
        const real_t f_4 = pdf.getF( &xyz0, 4);
        const real_t f_5 = pdf.getF( &xyz0, 5);
        const real_t f_6 = pdf.getF( &xyz0, 6);
        const real_t f_7 = pdf.getF( &xyz0, 7);
        const real_t f_8 = pdf.getF( &xyz0, 8);
        const real_t f_9 = pdf.getF( &xyz0, 9);
        const real_t f_10 = pdf.getF( &xyz0, 10);
        const real_t f_11 = pdf.getF( &xyz0, 11);
        const real_t f_12 = pdf.getF( &xyz0, 12);
        const real_t f_13 = pdf.getF( &xyz0, 13);
        const real_t f_14 = pdf.getF( &xyz0, 14);
        const real_t f_15 = pdf.getF( &xyz0, 15);
        const real_t f_16 = pdf.getF( &xyz0, 16);
        const real_t f_17 = pdf.getF( &xyz0, 17);
        const real_t f_18 = pdf.getF( &xyz0, 18);
        const double vel0Term = f_10 + f_14 + f_18 + f_4 + f_8;
        const double vel1Term = f_1 + f_11 + f_15 + f_7;
        const double vel2Term = f_12 + f_13 + f_5;
        const double rho = f_0 + f_16 + f_17 + f_2 + f_3 + f_6 + f_9 + vel0Term + vel1Term + vel2Term + 1;
        const double md_0 = -f_13 - f_17 - f_3 - f_7 - f_9 + vel0Term;
        const double md_1 = -f_10 - f_12 - f_16 - f_2 + f_8 - f_9 + vel1Term;
        const double md_2 = f_11 + f_14 - f_15 - f_16 - f_17 - f_18 - f_6 + vel2Term;
        momentumDensity[0] = md_0;
        momentumDensity[1] = md_1;
        momentumDensity[2] = md_2;
        
   }
};


template<>
struct PressureTensor<UniformGridGPU_LatticeModel>
{
   template< typename FieldPtrOrIterator >
   static void get( Matrix3< real_t > & /* pressureTensor */, const UniformGridGPU_LatticeModel & /* latticeModel */, const FieldPtrOrIterator & /* it */ )
   {
       WALBERLA_ABORT("Not implemented");
   }

   template< typename PdfField_T >
   static void get( Matrix3< real_t > & /* pressureTensor */, const UniformGridGPU_LatticeModel & /* latticeModel */, const PdfField_T & /* pdf */,
                    const cell_idx_t /* x */, const cell_idx_t /* y */, const cell_idx_t /* z */ )
   {
       WALBERLA_ABORT("Not implemented");
   }
};


template<>
struct ShearRate<UniformGridGPU_LatticeModel>
{
   template< typename FieldPtrOrIterator >
   static inline real_t get( const UniformGridGPU_LatticeModel & /* latticeModel */, const FieldPtrOrIterator & /* it */,
                             const Vector3< real_t > & /* velocity */, const real_t /* rho */)
   {
       WALBERLA_ABORT("Not implemented");
       return real_t(0.0);
   }

   template< typename PdfField_T >
   static inline real_t get( const UniformGridGPU_LatticeModel & latticeModel,
                             const PdfField_T & /* pdf */, const cell_idx_t /* x */, const cell_idx_t /* y */, const cell_idx_t /* z */,
                             const Vector3< real_t > & /* velocity */, const real_t /* rho */ )
   {
       WALBERLA_ABORT("Not implemented");
       return real_t(0.0);
   }

   static inline real_t get( const std::vector< real_t > & /* nonEquilibrium */, const real_t /* relaxationParam */,
                             const real_t /* rho */ = real_t(1) )
   {
       WALBERLA_ABORT("Not implemented");
       return real_t(0.0);
   }
};


} // namespace lbm
} // namespace walberla



#ifdef WALBERLA_CXX_COMPILER_IS_GNU
#pragma GCC diagnostic pop
#endif

#ifdef WALBERLA_CXX_COMPILER_IS_CLANG
#pragma clang diagnostic pop
#endif