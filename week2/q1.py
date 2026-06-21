import json
import copy  # use it for deepcopy if needed
import math  # for math.inf
import logging

logging.basicConfig(format='%(levelname)s - %(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                    level=logging.INFO)

# Global variables in which you need to store player strategies (this is data structure that'll be used for evaluation)
# Mapping from histories (str) to probability distribution over actions
strategy_dict_x = {}
strategy_dict_o = {}


class History:
    def __init__(self, history=None):
        """
        # self.history : Eg: [0, 4, 2, 5]
            keeps track of sequence of actions played since the beginning of the game.
            Each action is an integer between 0-8 representing the square in which the move will be played as shown
            below.
              ___ ___ ____
             |_0_|_1_|_2_|
             |_3_|_4_|_5_|
             |_6_|_7_|_8_|

        # self.board
            empty squares are represented using '0' and occupied squares are either 'x' or 'o'.
            Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
            for board
              ___ ___ ____
             |_x_|___|_x_|
             |___|_o_|_o_|
             |___|___|___|

        # self.player: 'x' or 'o'
            Player whose turn it is at the current history/board

        :param history: list keeps track of sequence of actions played since the beginning of the game.
        """
        if history is not None:
            self.history = history
            self.board = self.get_board()
        else:
            self.history = []
            self.board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        self.player = self.current_player()

    def current_player(self):
        """ Player function
        Get player whose turn it is at the current history/board
        :return: 'x' or 'o' or None
        """
        total_num_moves = len(self.history)
        if total_num_moves < 9:
            if total_num_moves % 2 == 0:
                return 'x'
            else:
                return 'o'
        else:
            return None

    def get_board(self):
        """ Play out the current self.history and get the board corresponding to the history in self.board.

        :return: list Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
        """
        board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        for i in range(len(self.history)):
            if i % 2 == 0:
                board[self.history[i]] = 'x'
            else:
                board[self.history[i]] = 'o'
        return board

    def get_winner(self):
        """ Helper: return the symbol ('x' or 'o') that has three in a row on self.board, else None. """
        lines = [
            (0, 1, 2), (3, 4, 5), (6, 7, 8),  # rows
            (0, 3, 6), (1, 4, 7), (2, 5, 8),  # cols
            (0, 4, 8), (2, 4, 6)              # diagonals
        ]
        for a, b, c in lines:
            if self.board[a] != '0' and self.board[a] == self.board[b] == self.board[c]:
                return self.board[a]
        return None

    def is_win(self):
        # check if the board position is a win for either players
        return self.get_winner() is not None

    def is_draw(self):
        # check if the board position is a draw
        return (not self.is_win()) and len(self.get_valid_actions()) == 0

    def get_valid_actions(self):
        # get the empty squares from the board
        return [i for i in range(9) if self.board[i] == '0']

    def is_terminal_history(self):
        # check if the history is a terminal history
        return self.is_win() or self.is_draw()

    def get_utility_given_terminal_history(self):
        # Utility is defined from player 'x' perspective (zero sum game):
        # +1 if x wins, -1 if o wins, 0 if draw.
        winner = self.get_winner()
        if winner == 'x':
            return 1
        elif winner == 'o':
            return -1
        else:
            return 0

    def update_history(self, action):
        # In case you need to create a deepcopy and update the history obj to get the next history object.
        new_history = copy.deepcopy(self.history)
        new_history.append(action)
        return History(history=new_history)


def backward_induction(history_obj):
    """
    :param history_obj: Histroy class object
    :return: best achievable utility (float) for th current history_obj
    """
    global strategy_dict_x, strategy_dict_o

    # Base case: terminal history, just return the utility (from x's perspective).
    # Nothing to store in strategy_dict_x / strategy_dict_o here since there is no
    # action to take at a terminal history.
    if history_obj.is_terminal_history():
        return history_obj.get_utility_given_terminal_history()

    player = history_obj.player
    history_key = ''.join(str(a) for a in history_obj.history)

    # Recurse into every child first (post-order / bottom-up), then decide the
    # best action for the current player once all children's values are known.
    action_values = {}
    for action in history_obj.get_valid_actions():
        next_history_obj = history_obj.update_history(action)
        action_values[action] = backward_induction(next_history_obj)

    # x maximizes the (x-perspective) utility, o minimizes it (zero-sum game).
    if player == 'x':
        best_value = max(action_values.values())
    else:
        best_value = min(action_values.values())

    # There could be multiple optimal actions; pick the smallest-indexed one and
    # put all the probability mass (1.0) on it, 0 on everything else, since
    # tictactoe (a PIEFG) always has a deterministic SPNE.
    best_action = min(a for a, v in action_values.items() if v == best_value)
    strategy = {str(a): (1.0 if a == best_action else 0.0) for a in range(9)}

    if player == 'x':
        strategy_dict_x[history_key] = strategy
    else:
        strategy_dict_o[history_key] = strategy

    return best_value


def solve_tictactoe():
    backward_induction(History())
    with open('./policy_x.json', 'w') as f:
        json.dump(strategy_dict_x, f)
    with open('./policy_o.json', 'w') as f:
        json.dump(strategy_dict_o, f)
    return strategy_dict_x, strategy_dict_o


if __name__ == "__main__":
    logging.info("Start")
    solve_tictactoe()
    logging.info("End")
