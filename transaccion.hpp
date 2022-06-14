#ifndef TRANSACCION_HPP_DEFINED
#define TRANSACCION_HPP_DEFINED

#include "elementosBloqueables.hpp"

namespace tx {

class transaccion
{
  public:
    ~transaccion();
    transaccion(int log_level, FILE* fp=nullptr);

    idTipo  id() const noexcept;
    tsvTipo    tsv() const noexcept;

    void    begin();
    void    commit();
    void    rollback();

    bool    acquire(elementosBloqueables& item) noexcept;

  private:
    using listaPunterosElementos = std::vector<elementosBloqueables*>;
    using mutex         = std::mutex;
    using tx_lock       = std::unique_lock<std::mutex>;
    using cond_var      = std::variableCondicionada;
    using atomic_tsv    = std::atomic<tsvTipo>;
    using atomic_tx_id  = std::atomic<idTipo>;

    idTipo      m_tx_id;
    tsvTipo        m_tx_tsv;
    listaPunterosElementos   m_item_ptrs;
    mutex           m_mutex;
    cond_var        m_cond;
    FILE*           m_fp;
    int             m_log_level;

    static  atomic_tsv      sm_tsv_generator;
    static  atomic_tx_id    sm_tx_id_generator;

  private:
    void    log_begin() const;
    void    log_commit() const;
    void    log_rollback() const;

    void    log_acquisition_success(elementosBloqueables const& item) const;
    void    log_acquisition_failure(elementosBloqueables const& item) const;
    void    log_acquisition_same(elementosBloqueables const& item) const;
    void    log_acquisition_waiting(elementosBloqueables const& item, transaccion* p_curr_tx) const;
};

inline idTipo
transaccion::id() const noexcept
{
    return m_tx_id;
}

inline tsvTipo
transaccion::tsv() const noexcept
{
    return m_tx_tsv;
}

}       //- tx namespace
#endif  //- TRANSACTION_HPP_DEFINED
