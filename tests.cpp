#include "transaccion.hpp"

using namespace tx;
using namespace std;
using namespace std::chrono_literals;

struct test_item;

using item_list = std::vector < test_item > ;
using index_list = std::vector < size_t > ;

using entropy = random_device;
using prn_gen = mt19937_64;
using int_dist = uniform_int_distribution < > ;
using hasher = hash < string_view > ;

struct test_item: public elementosBloqueables {
  static constexpr size_t buf_size = 32;

  char ma_chars[buf_size];

  void st_update(FILE * fp, prn_gen & gen, int_dist & char_dist);
  void tx_update(transaccion
    const & tx, FILE * fp, prn_gen & gen, int_dist & char_dist);
};

void
test_item::st_update(FILE * fp, prn_gen & gen, int_dist & char_dist) {
  char local_chars[buf_size];
  string_view local_view(local_chars, buf_size);
  string_view shared_view(ma_chars, buf_size);
  hasher hash;

  for (size_t i = 0; i < buf_size; ++i) {
    local_chars[i] = ma_chars[i] = (char) char_dist(gen);
  }

  if (hash(shared_view) != hash(local_view)) {
    fprintf(fp, "RC encontrada!, TX 0  elemento %zd\n", this -> id());
  }
}

void
test_item::tx_update(transaccion
  const & tx, FILE * fp, prn_gen & gen, int_dist & char_dist) {
  char local_chars[buf_size];
  string_view local_view(local_chars, buf_size);
  string_view shared_view(ma_chars, buf_size);
  hasher hash;

  for (size_t i = 0; i < buf_size; ++i) {
    local_chars[i] = ma_chars[i] = (char) char_dist(gen);
  }

  if (hash(shared_view) != hash(local_view)) {
    fprintf(fp, "RC encontrada!, TX %zd  elemento %zd\n", tx.id(), this -> id());
  }
}

void
st_access_test(item_list & items, FILE * fp, size_t impuestos, size_t tamReferencias) {
  entropy rd;
  prn_gen gen(rd());
  int_dist refs_index_dist(0, (int)(items.size() - 1));
  int_dist tamReferencias_dist(1, (int) tamReferencias);
  int_dist char_dist(0, 127);

  stopwatch sw;
  index_list indices;
  size_t index;

  sw.start();

  for (size_t i = 0; i < impuestos; ++i) {
    indices.clear();
    tamReferencias = tamReferencias_dist(gen);

    for (size_t j = 0; j < tamReferencias; ++j) {
      index = refs_index_dist(gen);
      indices.push_back(index);
    }

    for (size_t j = 0; j < tamReferencias; ++j) {
      index = indices[j];
      items[index].st_update(fp, gen, char_dist);
    }
  }

  sw.stop();
  fprintf(fp, "TX 0 tomo %d mseg\n", sw.milliseconds_elapsed < int > ());
}

void
mx_access_test(item_list & items, FILE * fp, size_t impuestos, size_t tamReferencias) {
  static mutex mtx;

  entropy rd;
  prn_gen gen(rd());
  int_dist refs_index_dist(0, (int)(items.size() - 1));
  int_dist tamReferencias_dist(1, (int) tamReferencias);
  int_dist char_dist(0, 127);

  stopwatch sw;
  index_list indices;
  size_t index;

  sw.start();

  for (size_t i = 0; i < impuestos; ++i) {
    indices.clear();
    tamReferencias = tamReferencias_dist(gen);

    for (size_t j = 0; j < tamReferencias; ++j) {
      index = refs_index_dist(gen);
      indices.push_back(index);
    }

    mtx.lock();

    for (size_t j = 0; j < tamReferencias; ++j) {
      index = indices[j];
      items[index].st_update(fp, gen, char_dist);
    }

    mtx.unlock();

    if (i < 10) {
      std::this_thread::yield();
    }
  }

  sw.stop();
  fprintf(fp, "TX 0 TOMO %d mseg\n", sw.milliseconds_elapsed < int > ());
}

void
tx_access_test(item_list & items, FILE * fp, size_t impuestos, size_t tamReferencias) {
  entropy rd;
  prn_gen gen(rd());
  int_dist refs_index_dist(0, (int)(items.size() - 1));
  int_dist tamReferencias_dist(1, (int) tamReferencias);
  int_dist char_dist(0, 127);

  transaccion tx(1, fp);
  stopwatch sw;
  index_list indices;
  size_t index;
  bool acquired;

  sw.start();

  for (size_t i = 0; i < impuestos; ++i) {
    indices.clear();
    tamReferencias = tamReferencias_dist(gen);

    for (size_t j = 0; j < tamReferencias; ++j) {
      index = refs_index_dist(gen);
      indices.push_back(index);
    }

    tx.begin();
    acquired = true;

    for (size_t j = 0; acquired && j < tamReferencias; ++j) {
      index = indices[j];
      acquired = (acquired && tx.acquire(items[index]));
    }

    if (acquired) {
      for (size_t j = 0; j < tamReferencias; ++j) {
        index = indices[j];
        items[index].tx_update(tx, fp, gen, char_dist);
      }
      tx.commit();
    } else {
      tx.rollback();
    }

    if (i < 10) {
      std::this_thread::yield();
    }
  }

  sw.stop();
  fprintf(fp, "TX %zd tomo %d msec\n", tx.id(), sw.milliseconds_elapsed < int > ());
}

void
ejecutarResultado
  (FILE * fp, size_t tamContainer, size_t tamThreads, size_t impuestos, size_t tamReferencias, size_t mod) {
    using future_list = std::vector < std::future < void >> ;

    stopwatch sw;
    future_list fv;

    if (mod == 0) {
      tamThreads = 0;
    } else {
      tamThreads = max((size_t) 1 u, tamThreads);
    }

    fprintf(fp, "Probado con\n");
    fprintf(fp, "  Elementos bloqueados: %zd\n", tamContainer);
    fprintf(fp, "  threads       : %zd\n", tamThreads);
    fprintf(fp, "  transacciones  : %zd\n", impuestos);
    fprintf(fp, "  refs per tx   : %zd\n", tamReferencias);

    sw.start();
    item_list items(tamContainer);
    sw.stop();
    fprintf(fp, "\nRecords inicializados requeridos %d mseg\n\n", sw.milliseconds_elapsed < int > ());

    sw.start();

    if (mod == 0) {
      st_access_test(items, fp, impuestos, tamReferencias);
    } else if (mod == 1) {
      for (size_t i = 0; i < tamThreads; ++i) {
        fv.push_back(std::async (std::launch::async,
          std::bind( & tx_access_test,
            std::ref(items),
            fp,
            impuestos,
            tamReferencias)));
      }
      for (size_t i = 0; i < tamThreads; ++i) {
        fv[i].wait();
      }
    } else if (mod == 2) {
      for (size_t i = 0; i < tamThreads; ++i) {
        fv.push_back(std::async (std::launch::async,
          std::bind( & mx_access_test,
            std::ref(items),
            fp,
            impuestos,
            tamReferencias)));
      }
      for (size_t i = 0; i < tamThreads; ++i) {
        fv[i].wait();
      }
    } else if (mod == 3) {
      for (size_t i = 0; i < tamThreads; ++i) {
        fv.push_back(std::async (std::launch::async,
          std::bind( & st_access_test,
            std::ref(items),
            fp,
            impuestos,
            tamReferencias)));
      }
      for (size_t i = 0; i < tamThreads; ++i) {
        fv[i].wait();
      }
    }
    sw.stop();
    fprintf(fp, "\ntransacciones requeridas %d mseg\n", sw.milliseconds_elapsed < int > ());
    sw.start();
    items.clear();
    sw.stop();
    fprintf(fp, "limpiezas necesarias %d mseg\n", sw.milliseconds_elapsed < int > ());
  }