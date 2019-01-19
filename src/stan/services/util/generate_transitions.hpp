#ifndef STAN_SERVICES_UTIL_GENERATE_TRANSITIONS_HPP
#define STAN_SERVICES_UTIL_GENERATE_TRANSITIONS_HPP

#include <stan/callbacks/iteration.hpp>
#include <stan/callbacks/log_iteration.hpp>
#include <stan/callbacks/interrupt.hpp>
#include <stan/callbacks/writer.hpp>
#include <stan/mcmc/base_mcmc.hpp>
#include <stan/services/util/mcmc_writer.hpp>
#include <string>

namespace stan {
  namespace services {
    namespace util {

      /**
       * Generates MCMC transitions.
       *
       * @tparam Model model class
       * @tparam RNG random number generator class
       * @param[in,out] sampler MCMC sampler used to generate transitions
       * @param[in] num_iterations number of MCMC transitions
       * @param[in] start starting iteration number used for printing messages
       * @param[in] finish end iteration number used for printing messages
       * @param[in] num_thin when save is true, a draw will be written to the
       *   mcmc_writer every num_thin iterations
       * @param[in] save if save is true, the transitions will be written
       *   to the mcmc_writer. If false, transitions will not be written
       * @param[in] warmup indicates whether these transitions are warmup. Used
       *   for printing iteration number messages
       * @param[in,out] mcmc_writer writer to handle mcmc otuput
       * @param[in,out] init_s starts as the initial unconstrained parameter
       *   values. When the function completes, this will have the final
       *   iteration's unconstrained parameter values
       * @param[in] model model
       * @param[in,out] base_rng random number generator
       * @param[in,out] callback interrupt callback called once an iteration
       * @param[in,out] iteration iteration callback called with once an iteration with
       *   the iteration number (1-indexed)
       * @param[in,out] logger logger for messages
       */
      template <class Model, class RNG>
      void generate_transitions(stan::mcmc::base_mcmc& sampler,
                                int num_iterations, int start,
                                int finish, int num_thin,
                                bool save, bool warmup,
                                util::mcmc_writer& mcmc_writer,
                                stan::mcmc::sample& init_s,
                                Model& model, RNG& base_rng,
                                callbacks::interrupt& callback,
                                callbacks::iteration& iteration,
                                callbacks::logger& logger) {
        for (int m = 0; m < num_iterations; ++m) {
          callback();
          iteration(start + m + 1);

          init_s = sampler.transition(init_s, logger);

          if (save && ((m % num_thin) == 0)) {
            mcmc_writer.write_sample_params(base_rng, init_s, sampler, model);
            mcmc_writer.write_diagnostic_params(init_s, sampler);
          }
        }
      }


      /**
       * Generates MCMC transitions.
       *
       * @tparam Model model class
       * @tparam RNG random number generator class
       * @param[in,out] sampler MCMC sampler used to generate transitions
       * @param[in] num_iterations number of MCMC transitions
       * @param[in] start starting iteration number used for printing messages
       * @param[in] finish end iteration number used for printing messages
       * @param[in] num_thin when save is true, a draw will be written to the
       *   mcmc_writer every num_thin iterations
       * @param[in] refresh number of iterations to print a message. If
       *   refresh is zero, iteration number messages will not be printed
       * @param[in] save if save is true, the transitions will be written
       *   to the mcmc_writer. If false, transitions will not be written
       * @param[in] warmup indicates whether these transitions are warmup. Used
       *   for printing iteration number messages
       * @param[in,out] mcmc_writer writer to handle mcmc otuput
       * @param[in,out] init_s starts as the initial unconstrained parameter
       *   values. When the function completes, this will have the final
       *   iteration's unconstrained parameter values
       * @param[in] model model
       * @param[in,out] base_rng random number generator
       * @param[in,out] callback interrupt callback called once an iteration
       * @param[in,out] logger logger for messages
       */
      template <class Model, class RNG>
      void generate_transitions(stan::mcmc::base_mcmc& sampler,
                                int num_iterations, int start,
                                int finish, int num_thin, int refresh,
                                bool save, bool warmup,
                                util::mcmc_writer& mcmc_writer,
                                stan::mcmc::sample& init_s,
                                Model& model, RNG& base_rng,
                                callbacks::interrupt& callback,
                                callbacks::logger& logger) {
        callbacks::log_iteration iteration(logger, warmup ? finish : start,
                                           finish, refresh);
        generate_transitions(sampler, num_iterations, start, finish, num_thin,
                             save, warmup, mcmc_writer, init_s, model,
                             base_rng, callback, iteration, logger);
      }

    }
  }
}

#endif
