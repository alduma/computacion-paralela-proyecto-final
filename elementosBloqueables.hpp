#ifndef ELEMENTOS_BLOQUEABLES_HPP_DEFINED
#define ELEMENTOS_BLOQUEABLES_HPP_DEFINED

#include "forward.hpp"

namespace tx {

  class elementosBloqueables {
    public:
      elementosBloqueables();

    item_id_type id() const noexcept;
    tsvTipo last_tsv() const noexcept;

    private:
      friend class transaccion;

    using tx_pointer = std::atomic < transaccion * > ;
    using atomic_item_id = std::atomic < item_id_type > ;

    tx_pointer mp_owning_tx;
    tsvTipo m_last_tsv;
    item_id_type m_item_id;

    static atomic_item_id sm_item_id_generator;
  };

  inline
  elementosBloqueables::elementosBloqueables(): mp_owning_tx(nullptr), m_last_tsv(0), m_item_id(++sm_item_id_generator) {}

  inline item_id_type
  elementosBloqueables::id() const noexcept {
    return m_item_id;
  }

  inline tsvTipo
  elementosBloqueables::last_tsv() const noexcept {
    return m_last_tsv;
  }

}
#endif